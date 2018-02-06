import React from 'react'
import { render } from 'react-dom'
import localForage from 'localforage'
import injectTapEventPlugin from 'react-tap-event-plugin'
import DefaultSettings from './App/default_settings.json'
import App from './App'

// first, attempt to register service worker
// and set a flag when done, even if it failed...
var swLoaded = false
window.addEventListener('load', () => {
    navigator.serviceWorker.register('/sw.js').then(resolve => {
        swLoaded = true
    }).catch(error => {
        console.error(error)
        swLoaded = true
    })
})

// then, render app...
const serviceWorkerLoaded = () => { return swLoaded }
localForage.getItem("settings").then(settings => {
    if(!settings)
        return localForage.setItem("settings", DefaultSettings)
    else
        return Promise.resolve()
})
.then(() => render( <App swLoaded={serviceWorkerLoaded} />, document.getElementById('app') ) )
.catch(console.error)
