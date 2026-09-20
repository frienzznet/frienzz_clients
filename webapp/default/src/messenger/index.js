/**
 * Class to manage the websocket communications
 */

class Messenger {
  constructor() {
    this.modules = {} /* contains the registered modules */



    // Stores the initial state using the `getState()` function (defined above).
    let state = this.getState();

    // Accepts a next state and, if there's been a state change, logs the
    // change to the console. It also updates the `state` value defined above.
    const logStateChange = (nextState) => {
      const prevState = state;
      if (nextState !== prevState) {
        console.log(`State change: ${prevState} >>> ${nextState}`);
        state = nextState;
        if (state == 'frozen') {
          this.disconnect();
        }
        if (state == 'resume') {
          this.connect();
        }
      }
    };

    // These lifecycle events can all use the same listener to observe state
    // changes (they call the `getState()` function to determine the next state).
    ['pageshow', 'focus', 'blur', 'visibilitychange', 'resume'].forEach((type) => {
      window.addEventListener(type, () => logStateChange(this.getState()), { capture: true });
    });

    // The next two listeners, on the other hand, can determine the next
    // state from the event itself.
    window.addEventListener('freeze', () => {
      // In the freeze event, the next state is always frozen.
      logStateChange('frozen');
    }, { capture: true });

    window.addEventListener('pagehide', (event) => {
      if (event.persisted) {
        // If the event's persisted property is `true` the page is about
        // to enter the Back-Forward Cache, which is also in the frozen state.
        logStateChange('frozen');
      } else {
        // If the event's persisted property is not `true` the page is
        // about to be unloaded.
        logStateChange('terminated');
      }
    }, { capture: true });



    console.log("Construct Messenger")

  }

  getState() {
    if (document.visibilityState === 'hidden') {
      return 'hidden';
    }
    if (document.hasFocus()) {
      return 'active';
    }
    return 'passive';
  }


  register(prefix, wsMsghandler, wsOnOpen, wsOnClose) {
    this.modules[prefix] = { 'msg': wsMsghandler, 'open': wsOnOpen, 'close': wsOnClose }
    console.log("Register prefix", prefix, this.modules)
  }


  handleMessage(data) {
    var success = false
    if (data) {
      if ('token' in data) {
        this.token = data.token
      }
      for (let prefix in this.modules) {
        if ('type' in data && 'config' in data && data.type.startsWith(prefix)) {
          this.modules[prefix].msg(data.type, data.config)
          success = true
          break
        }
      }
    }
    if (!success) {
      console.log("Error: Unknown ws message type ", data.type)
    }
  }

  init(token) {
    this.token = token
    this.connect()
  }

}

/*
class WebService  extends Messenger{

  constructor() {
    super();
    let ws_protocol = "ws://"
    if (window.location.protocol == "https:") {
      ws_protocol = "wss://"
    }
    //this.SIGNALING_SERVER = ws_protocol + window.location.hostname + ":" + window.location.port
    this.SIGNALING_SERVER = ws_protocol + window.location.hostname + ":8000"
    this.signaling_socket = null   // our socket.io connection to our webserver 
    //alias for sending JSON encoded messages
    this.emit = (type, config) => {
      //attach the other peer username to our messages 
      let message = { 'type': type, 'config': config }
      if (this.signaling_socket && this.signaling_socket.readyState == 1) {
        this.signaling_socket.send(JSON.stringify(message))
      }
    }

    console.log("Construct WebService", this.modules)

  }



  connect() {
    let wshandler = this
    console.log("Connecting to signaling server")
    this.signaling_socket = new WebSocket(this.SIGNALING_SERVER)
    this.signaling_socket.onopen = () => {
      console.log("Connected to the signaling server")
      for (let prefix in this.modules) {
        if (this.modules[prefix].open) {
          this.modules[prefix].open()
        }
      }
      this.emit('_join', { "name": this.username })
    }

    this.signaling_socket.onclose = (e) => {
      console.log("Disconnected from signaling server")
      for (let prefix in this.modules) {
        if (this.modules[prefix].close) {
          this.modules[prefix].close()
        }
      }
      console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
      setTimeout(function () {
        wshandler.connect();
      }, 1000);
    }

    //when we got a message from a signaling server 
    this.signaling_socket.onmessage = (msg) => {
      var data = JSON.parse(msg.data)
      this.handleMessage(data)

    }

    this.signaling_socket.onerror = function (err) {
      console.error('Socket encountered error: ', err.message, 'Closing socket');
      this.signaling_socket = null
    }

  }

  disconnect() {
    if (this.signaling_socket) {
      this.signaling_socket.close()
      this.signaling_socket = null
    }
  }
}

*/

class FetchService extends Messenger {

  constructor() {
    super();
    console.log("Construct FetchService", this.modules)

  }



  connect() {
    console.log("Connecting to signaling server")
    /* as Fetch is a stateless connection, we'll declare an connection :-) */
    for (let prefix in this.modules) {
      if (this.modules[prefix].open) {
        this.modules[prefix].open()
      }
    }
    this.emit('home_status', {})


    /* as Fetch is a stateless connection, we'll never have a disconnect :-)
      console.log("Disconnected from signaling server")
      for (let prefix in this.modules) {
        if (this.modules[prefix].close) {
          this.modules[prefix].close()
        }
      }
      console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
      */
  }

  emit(type, config) {
    //const fetch_url = 'http://steffen-UDPC-1:8000/xhr.php'
    // const fetch_url = 'http://192.168.1.185:8000/xhr.php'
    const fetch_url = 'xhr.php'
    //attach the other peer username to our messages 
    let message = { 'type': type, 'config': config }
    if (this.token) {
      message.token = this.token
      // a good moment to forward the token to the service worker, so it can use it for authentication in fetch requests
      this.sendNotification("parameters", { token: this.token, fetch_url: fetch_url })
    }
    const requestOptions = {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(message)
    };
    try {
      fetch(fetch_url, requestOptions)
        .then(response => response.json(), error => console.log(error))
        .then(data => this.handleMessage(data), error => console.log(error))
    } catch (error) {
      console.group(error)
    }


  }

  // Create and send a test notification to the service worker.
  sendNotification(type, config) {

    let notification = {
      type: type,
      config: config
    };
    if (!navigator.serviceWorker) {
      console.log('Service workers are not supported by this browser.');
      return;
    }
    // Get a reference to the service worker registration.
    navigator.serviceWorker.getRegistration().then(registration => {

      // Check that a service worker controller exists before
      // trying to access the postMessage method.
      console.log("getRegistration", registration);
      if (navigator.serviceWorker.controller) {
        console.log("postMessage to ServiceWorker", notification);
        navigator.serviceWorker.controller.postMessage(notification);
      }
    }, error =>
      console.log('No service worker controller found. Try a soft reload.', error)
    )

  }

  disconnect() {

  }
}




//const messenger = new WebService()
const messenger = new FetchService()

export default messenger

