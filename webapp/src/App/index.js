import React from 'react'
import ScriptJS from 'scriptjs'
import EmulatorScreen from './EmulatorScreen'
import DefaultState from './default_state.json'
import './app.scss'

class App extends React.Component {
    constructor(props) {
        super(props)
        const self  = this
        self.state  = Object.assign({}, DefaultState);
        const start = new Promise(resolve => {
            // load wasm and wait for it to fully load before moving on...
            ScriptJS("bin.js", resolve)
        })
        .then(() => {
            // wait for the wasm runtime to be initiated before moving on...
            return new Promise(resolve => {
                Module['onRuntimeInitialized'] = resolve
            })
        })
        .then(() => {
            // finally, render emulator screen
            self.setState({ runtimeInitialized: true })
        })
        .catch(console.error)

    }
    startEmulation() {
        this.setState({ emulatorRunning: true })
    }
    stopEmulation() {
        Module.ccall('halt')
        this.setState({ emulatorRunning: false })
    }
    render () {
        if(this.state.runtimeInitialized) {
            if(this.state.emulatorRunning) {
                var page = <EmulatorScreen />
            }
            else {
                var page = (
                    <button onClick={this.startEmulation.bind(this)}>
                        {"Start!"}
                    </button>
                )
            }
        }
        else {
            var page = <div>loading...</div>
        }
        return (
            <div id={"ui_container"}>
                {page}
            </div>
        )
    }
}

module.exports = App
