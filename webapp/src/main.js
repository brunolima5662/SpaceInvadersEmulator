import React from 'react'
import { render } from 'react-dom'
import localForage from 'localforage'
import injectTapEventPlugin from 'react-tap-event-plugin'
import DefaultState from './App/default_state.json'
import App from './App'

localForage.getItem("settings").then(settings => {
    if(!settings)
        return localForage.setItem("settings", DefaultState["settings"])
    else
        return Promise.resolve()
})
.then(() => {
    render( <App />, document.getElementById('app') )
})
.catch(console.error)
