import React from 'react'
import PropTypes from 'prop-types'
import localForage from 'localforage'
import Transition from 'react-transition-group/Transition'
import Snackbar from '../Snackbar'
import _ from 'lodash'
import { jsArray, cArray, freeCArray, rgb332, contrastColor } from '../Modules/helpers'
import './emulator_screen.scss'
const theme  = require('../../../theme.json')
const config = require('../config.json')

class EmulatorScreen extends React.Component {
    constructor(props) {
        super(props)
        this.state = { paused: false, saved: false, saving: false, savedSnackbar: false }
    }
    onCanvasLoaded(canvas) {
        if(!this.loaded) {
            this.loaded = true
            const self  = this
            const shouldLoadState = this.props.shouldLoadState
            var load = [ localForage.getItem("settings") ]
            if(shouldLoadState) {
                load.push( localForage.getItem("saved_state") )
                load.push( localForage.getItem("saved_machine") )
            }
            else {
                load.push( Promise.resolve(null) )
                load.push( Promise.resolve(null) )
            }
            Promise.all(load).then(([ settings, state, machine ]) => {
                try {
                    const types = config["emulator_arg_types"]
                    const set_foreground   = settings["foreground"] != "#ffffff" ? 1 : 0
                    const set_background   = settings["background"] != "#000000" ? 1 : 0
                    const should_add_state = ( shouldLoadState && state && machine )
                    const args = [
                        ( ( set_foreground << 1 ) | set_background ), // set_colors_mask
                        rgb332( settings["foreground"] ), // foreground
                        rgb332( settings["background"] ), // background
                        settings["lives"], // lives
                        should_add_state ? cArray( state ) : 0, // saved_state
                        should_add_state ? cArray( machine ) : 0 // saved_machine
                    ]

                    self.setState({
                        foreground: set_foreground ? settings["foreground"] : null,
                        background: set_background ? settings["background"] : null,
                        text: set_foreground ? contrastColor( settings["foreground"] ) : null
                    })
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
                        console.error(error)
                }
            })
        }
    }
    pause() {
        this.setState({
            paused: !this.state.paused,
            saved: false,
            saving: false
        }, () => {
            // pause/resume game here...
            Module.ccall('toggle_pause', null, null)
        })
    }
    save() {
        const self = this
        this.setState({ saved: true, saving: true }, () => {
            const bytes   = 0x10000
            const pointer = Module.ccall('get_state', ['number'], null)
            const state   = jsArray(pointer, bytes)
            localForage.setItem("saved_state", state).then(() => {
                const m_pointer = Module.ccall('get_machine', ['number'], null)
                const machine   = jsArray(m_pointer, 35)
                return localForage.setItem("saved_machine", machine).then(() => {
                    return Promise.resolve(machine)
                })
            })
            .then(machine => {
                return new Promise(resolve => {
                    freeCArray(machine)
                    self.setState({ savedSnackbar: true, saving: false }, resolve)
                })
            })
            .then(() => {
                setTimeout(() => { self.setState({ savedSnackbar: false }) }, 2500)
            })

        })

    }
    render() {
        return (
            <div className={"emulator-screen"} style={{backgroundColor: this.state.background}}>
                <canvas
                    ref={this.onCanvasLoaded.bind(this)}
                    height={theme["render-height"]}
                    width={theme["render-width"]}>
                </canvas>
                <button
                className={"quit"}
                style={{  backgroundColor: this.state.foreground, color: this.state.text }}
                onClick={this.props.onQuit}
                >
                    <i className="material-icons">home</i>
                </button>
                <Transition in={(this.state.paused && !this.state.saved)} timeout={100}>
                    {state => (
                        <button
                        className={`save ${state}`}
                        style={{  backgroundColor: this.state.foreground, color: this.state.text }}
                        onClick={this.save.bind(this)}
                        >
                            <i className="material-icons">save</i>
                        </button>
                    )}
                </Transition>
                <button
                className={"pause"}
                style={{
                    backgroundColor: !this.state.saving ? this.state.foreground : '#BCBCBC',
                    color: !this.state.saving ? this.state.text : 'white'
                }}
                disabled={this.state.saving}
                onClick={this.pause.bind(this)}
                >
                    {!this.state.paused && <i className="material-icons">pause</i>}
                    {this.state.paused && <i className="material-icons">play_arrow</i>}
                </button>
                <Snackbar
                className={"snackbar"}
                style={{  backgroundColor: this.state.foreground, color: this.state.text }}
                display={this.state.savedSnackbar}
                >
                    {"Game Saved!"}
                </Snackbar>
            </div>
        )
    }
}

EmulatorScreen.propTypes = {
    shouldLoadState: PropTypes.bool,
    onQuit: PropTypes.func.isRequired
}

module.exports = EmulatorScreen
