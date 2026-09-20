/* eslint-disable no-console */

import { register } from 'register-service-worker'
console.log("debug ", process.env.NODE_ENV, process.env.BASE_URL, register)
if (process.env.NODE_ENV === 'production') {
  register(`${process.env.BASE_URL}frienzz-service-worker.js`, {
    ready() {
      console.log(
        'App is being served from cache by a service worker.\n' +
        'For more details, visit https://goo.gl/AFskqB'
      )
      // prepare the service worker for periodic background sync
      if ('serviceWorker' in navigator) {
        console.log('ServiceWorker is in navigator')
        navigator.serviceWorker.ready.then(registration => {
          console.log('ServiceWorker is ready')
          if ('periodicSync' in registration) {
            console.log('periodicSync in registration')

            try {
              registration.periodicSync.register('frienzz-sync', {
                minInterval: 5 * 60 * 1000, // 5 mins
              })
              console.log('periodicSync is registered')
            } catch (error) {
              console.error('Periodic Sync could not be registered!', error)
            }
          }
        })
      }



    },
    registered() {
      console.log('Service worker has been registered.')
    },
    cached() {
      console.log('Content has been cached for offline use.')
    },
    updatefound() {
      console.log('New content is downloading.')
    },
    updated() {
      console.log('New content is available; please refresh.')
    },
    offline() {
      console.log('No internet connection found. App is running in offline mode.')
    },
    error(error) {
      console.error('Error during service worker registration:', error)
    }
  })
}
