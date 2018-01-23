import React from 'react'

import './emulator_screen.scss'
const theme = require('../../../theme.json')

class EmulatorScreen extends React.Component {
    onCanvasLoaded(canvas) {
        if(!this.loaded) {
            this.loaded = true
            try {
                Module['canvas'] = canvas
                Module.ccall('mainf', 'number', null, null)
            }
            catch(error) {
                // suppress the 'SimulateInfiniteLoop' error since it's
                // not really an error, it's the mechanism that emscripten
                // uses to be able to simulate an infinite loop in a JS
                // environment. If it's not caught, it will break the
                // React component.
                if(error !== "SimulateInfiniteLoop")
                    console.error(error.message)
            }
        }
    }
    render() {
        return (
            <canvas
                ref={this.onCanvasLoaded.bind(this)}
                height={theme["render-height"]}
                width={theme["render-width"]}>
            </canvas>
        )
    }
}

module.exports = EmulatorScreen
