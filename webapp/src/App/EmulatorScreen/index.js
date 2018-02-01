import React from 'react'
import PropTypes from 'prop-types'
import _ from 'lodash'
import './emulator_screen.scss'
const theme  = require('../../../theme.json')
const config = require('../config.json')

class EmulatorScreen extends React.Component {
    constructor(props) {
        super(props)
        if(!this.props.shouldLoadState)
            this.state = { paused: false, saved: false }
        else
            ; // load state here
    }
    onCanvasLoaded(canvas) {
        const self = this

        if(!this.loaded) {
            this.loaded = true
            try {
                const types = config["emulator_arg_types"]
                const args  = _.reduce(config["emulator_args"], (a, v, k) => {
                    const setting = self.props.settings[k]
                    if(setting)
                        a[v] = setting
                    else
                        a[v] = 0
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
    pause() {
        this.setState({ paused: !this.state.paused, saved: false }, () => {
            // pause/resume game here...
            Module.ccall('toggle_pause', null, null)
        })
    }
    save() {
        this.setState({ saved: true }, () => {
            // save state here...
        })
    }
    render() {
        return (
            <div className={"emulator-screen"}>
                <div className={"screen"}>
                    <canvas
                        ref={this.onCanvasLoaded.bind(this)}
                        height={theme["render-height"]}
                        width={theme["render-width"]}>
                    </canvas>
                    <button className={"quit"} onClick={this.props.onQuit}>
                        <i className="material-icons">home</i>
                    </button>
                    {(this.state.paused && ! this.state.saved) &&
                        <button className={"save"} onClick={this.save.bind(this)}>
                            <i className="material-icons">save</i>
                        </button>
                    }
                    <button className={"pause"} onClick={this.pause.bind(this)}>
                        {!this.state.paused && <i className="material-icons">pause</i>}
                        {this.state.paused && <i className="material-icons">play arrow</i>}
                    </button>
                </div>
            </div>
        )
    }
}

EmulatorScreen.propTypes = {
    shouldLoadState: PropTypes.bool,
    onQuit: PropTypes.func.isRequired
}

module.exports = EmulatorScreen
