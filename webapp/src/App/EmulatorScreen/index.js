import React from 'react'
import PropTypes from 'prop-types'
import _ from 'lodash'
import './emulator_screen.scss'
const theme  = require('../../../theme.json')
const config = require('../../../config.json')

class EmulatorScreen extends React.Component {
    onCanvasLoaded(canvas) {
        const self = this
        if(!this.loaded) {
            this.loaded = true
            try {
                const types = config["emulator_arg_types"]
                const args  = _.reduce(config["emulator_args"], (a, k, v) => {
                    a[v] = self.props.settings[k]
                    return a
                }, new Array(types.length))
                Module['canvas'] = canvas
                Module.ccall('mainf', 'number', types, args)
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

EmulatorScreen.propTypes = {
    settings: PropTypes.object.isRequired
}

module.exports = EmulatorScreen
