import React from 'react'
import PropTypes from 'prop-types'
import ScriptJS from 'scriptjs'
import localForage from 'localforage'
import HomeScreen from './HomeScreen'
import EmulatorScreen from './EmulatorScreen'
import { malloc } from './Modules/helpers'
import './app.scss'

class App extends React.Component {
    constructor(props) {
        super(props)
        const self  = this
        this.state = {
            runtimeInitialized: false,
            emulatorRunning: false,
            shouldLoadState: false
        }
        const start = new Promise(resolve => {
            // wait for service worker to register before continuing...
            var checkServiceWorker = setInterval(() => {
                if(self.props.swLoaded()) {
                    clearInterval(checkServiceWorker)
                    resolve()
                }
            })
        })
        .then(() => {
            // then, load wasm and wait for it to fully load before moving on...
            return new Promise( resolve =>  ScriptJS("/bin.js", resolve) )
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
        // the emulator will dispatch an event that the following
        // method will handle when it finishes on its own...
        this.onEmulatorStopped = ev => window.location.reload(false)

    }
    componentDidMount() {
        document.addEventListener("emulator_stop", this.onEmulatorStopped)
    }

    componentWillUnmount() {
        document.removeEventListener("emulator_stop", this.onEmulatorStopped)
    }
    startEmulation(shouldLoadState = false) {
        this.setState({ emulatorRunning: true, shouldLoadState })
    }
    stopEmulation() {
        Module.ccall('halt')
        window.location.reload(false)
    }
    render () {
        return (
            <div id={"ui_container"}>
                {(this.state.runtimeInitialized && this.state.emulatorRunning) &&
                    <EmulatorScreen
                        onQuit={this.stopEmulation.bind(this)}
                        shouldLoadState={this.state.shouldLoadState}
                    />
                }
                {(this.state.runtimeInitialized && !this.state.emulatorRunning) &&
                    <HomeScreen onStart={this.startEmulation.bind(this)} />
                }
                {!this.state.runtimeInitialized &&
                    <div id={"loader"}>
                        <div className={"loader"}></div>
                        <div className={"label"}>{"loading..."}</div>
                    </div>
                }
            </div>
        )
    }
}

App.propTypes = {
    swLoaded: PropTypes.func.isRequired
}

module.exports = App
