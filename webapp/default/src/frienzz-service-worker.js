
// How to create a service worker with workbox: https://swina.github.io/2019/02/vue-service-worker-for-webpush-notifications/


self.__precacheManifest = [].concat(self.__precacheManifest || []);
// ??? workbox.precaching.suppressWarnings();
workbox.precaching.precacheAndRoute(self.__precacheManifest, {});


/*
  const options = {
    body: push_message.notification.body,
    icon: push_message.notification.icon,
    image: push_message.notification.image,
    tag: 'alert'
  };
  event.waitUntil(self.registration.showNotification(push_message.notification.title, options));
*/

console.log('inside the service worker')

// send data to service worker: https://stackoverflow.com/a/51563625
// https://web.dev/articles/codelab-notifications-service-worker?hl=de#send_notifications_to_the_service_worker


let friends_states = new Map() // we keep the latest friend states in the service worker, so that we can easily check for changes and show notifications
let token = "" // we also keep the token in the service worker, so that we can easily send it to the server when we need to request the friend states
let fetch_url = "" // we also keep the fetch url in the service worker, so that we can easily send it to the server when we need to request the friend states

function push_notification(friend_available_type, friend_available_time, friend_nickname, friend_uuid) {
    // https://developer.mozilla.org/en-US/docs/Web/API/Notifications_API/Using_the_Notifications_API#creating_a_notification
    const state_symbols = ["", "📞", "🏠", "☕", "🍸", "🍖", "🎉"]
    console.log("show notification");
    const img = "img/icons/android-icon-96x96.png";
    const absolute_available_time = friend_available_time + Math.floor((new Date()).getTime() / 1000)
    var date = new Date(absolute_available_time * 1000);
    message = state_symbols[friend_available_type] + friend_nickname + " ↠  " + date.toLocaleTimeString()
    //const notification = new Notification("To do list", { body: text, icon: img });
    self.registration.showNotification(
        "Frienzz",
        { body: message, icon: img }
    ).catch((error) => {
        console.log(error);
    });
    //new Notification("To do list", { body: message, icon: img });
}


function friends_notifications(states) {
    console.log("check friend states for notifications", states);
    var new_friend_states = new Map() // while we checking, we also create the latest state map, so that we can easily update the friend states after the loop
    for (const state of states) {
        const friend_uuid = state.uuid
        const friend_nickname = state.friend_nickname
        const friend_available_time = state.available_time
        const friend_available_type = state.available_type
        new_friend_states.set(friend_uuid, { friend_nickname: friend_nickname, friend_available_time: friend_available_time, friend_available_type: friend_available_type })
        if (friends_states.size > 0) {
            if (friends_states.has(friend_uuid)) {
                const old_state = friends_states.get(friend_uuid)
                console.log("check friend state for notifications", old_state);
                if (old_state.friend_available_time == 0 && friend_available_time > 0 && old_state.friend_available_time != friend_available_time) {
                    // friend just became available
                    push_notification(friend_available_type, friend_available_time, friend_nickname, friend_uuid)
                }
                else {
                    console.log("check friend state for notifications, check for available type change", old_state);
                    if (old_state.friend_available_type != friend_available_type && friend_available_time > 0) {
                        // friend just changed his available type
                        push_notification(friend_available_type, friend_available_time, friend_nickname, friend_uuid)
                    }
                }
            } else {
                // new friend, we just show a notification if he is available
                push_notification("new friend " + friend_nickname + " is now available!")
            }
        }
    }
    friends_states = new_friend_states;
}


async function evaluate_request(request) {
    console.log("evaluate_request done");
    url = request.url
    console.log("evaluate_request url", url);
    if (!url.endsWith("/xhr.php")) {
        console.log("no xhr request", url);
        return fetch(request)
    }
    console.log("evaluate_xhr_request");
    const responseFromNetwork = await fetch(request);
    const responseClone = responseFromNetwork.clone();
    const responseJson = await responseClone.json();
    console.log("evaluate_xhr_request responseJson", responseJson);
    try {
        friends_notifications(responseJson.config.status.friends);
    }
    catch (e) {
        console.log("error in evaluate_xhr_request", e);
    }
    return responseFromNetwork
}

self.addEventListener("fetch", (event) => {
    console.log(`Handling fetch event for ${event.request.url}`);
    event.respondWith(evaluate_request(event.request));
})


// Source - https://stackoverflow.com/a/51563625
// Posted by binarybossdhi, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-20, License - CC BY-SA 4.0

self.addEventListener('activate', function (event) {
    console.log('Claiming control');
    return self.clients.claim();
});

// Listen for messages from the main thread https://web.dev/articles/codelab-notifications-service-worker?hl=de#send_notifications_to_the_service_worker

// Show notification when received
self.addEventListener('message', (event) => {
    console.log("Received post message in service worker:", event.data)
    try {
        if (event.data.type === 'parameters') {
            token = event.data.config.token
            fetch_url = event.data.config.fetch_url
        }
    }
    catch (e) {
        console.log("error in post message event listener", e);
    }
});

self.addEventListener('install', function (event) {
    console.log('Service worker installing...');
    return self.skipWaiting();
});

self.addEventListener('activate', event => {
    event.waitUntil(
        caches.keys().then(cacheNames => {
            return Promise.all(
                cacheNames.map(cache => {
                    if (cache !== 'current-cache-version') {
                        return caches.delete(cache); // Delete old cache
                    }
                })
            );
        })
    );
});

// Source - https://stackoverflow.com/q/70982845
// Posted by Pavel Třupek, modified by community. See post 'Timeline' for change history
// Retrieved 2026-03-01, License - CC BY-SA 4.0

async function do_fetch_home_status() {
    console.log('updateState called in service worker');
    let message = { 'type': 'home_status', 'config': {}, token: token }

    const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(message)
    };
    try {
        fetch(fetch_url, requestOptions)
            .then(response => response.json(), error => console.log(error))
            .then(data => this.friends_notifications(data.config.status.friends), error => console.log(error))
    } catch (error) {
        console.group(error)
    }

}

self.addEventListener('periodicsync', (event) => {
    console.log('periodicSync event received in service worker', event);

    if (event.tag === 'frienzz-sync') {
        event.waitUntil(do_fetch_home_status());
    }
});

