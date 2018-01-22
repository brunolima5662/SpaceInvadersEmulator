import React from 'react'
import { render } from 'react-dom'
import ScriptJS from 'scriptjs'
import injectTapEventPlugin from 'react-tap-event-plugin'

import App from './App'

const LoadScript = s => new Promise( r => ScriptJS( s, r ) )

const startup = new Promise((resolve, reject) => {
    // start by rendering app, only continue after
    // canvas DOM object has rendered
    render(<App onCanvasReady={resolve} /> , document.getElementById('app') )
})
.then(canvas => Promise.all([ Promise.resolve(canvas), LoadScript("bin.js") ]))
.then(([ canvas, script ]) => {
    Module['canvas'] = canvas
    Module['onRuntimeInitialized'] = () => {
        // start emulator
        Module.ccall('mainf', 'number', null, null)
    }
})
.catch(console.error)
