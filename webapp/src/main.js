import React from 'react'
import { render } from 'react-dom'
import ScriptJS from 'scriptjs'
import injectTapEventPlugin from 'react-tap-event-plugin'

import App from './App'

const startup = new Promise((resolve, reject) => {
    // start by rendering app, only continue after
    // canvas DOM object has rendered
    const onCanvasReady = canvas => {
        console.log('canvas rendered...')
        resolve(canvas)
    }
    console.log("rendering...")
    // Render!
    render(<App onCanvasReady={onCanvasReady} /> , document.getElementById('app') )
})
.then(canvas => {
    // create emscripten Module object and load compiled
    // emulator script...
    return Promise.all([
        Promise.resolve(canvas),
        new Promise(r => ScriptJS("bin.js", r))
    ])
})
.then(([ canvas, script ]) => {
    console.log("script loaded, calling mainf...")
    // script has loaded, start emulator...
    Module.canvas = () => canvas
    Module.ccall('mainf', null, null)
})
.catch(console.error)
