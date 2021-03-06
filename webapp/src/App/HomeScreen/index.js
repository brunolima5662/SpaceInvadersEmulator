import React from 'react'
import localForage from 'localforage'
import DefaultSettings from '../default_settings.json'
import ColorSelector from '../ColorSelector'
import { generateRGB332Palette } from '../Modules/helpers'
import logo from './logo.png'
import heart from './heart.png'
import './home_screen.scss'

const palette = generateRGB332Palette()

class HomeScreen extends React.Component {
    constructor(props) {
        super(props)
        this.state = {
            settings: DefaultSettings,
            hasSavedState: false,
            selectingForeground: false,
            selectingBackground: false,
            tempForeground: "#FFFFFF",
            tempBackground: "#000000"
        }
        Promise.all([
            localForage.getItem("settings"),
            localForage.getItem("saved_state"),
            localForage.getItem("saved_machine")
        ]).then(this.dataLoaded.bind(this))
    }
    dataLoaded([ settings, savedState, savedMachine ]) {
        const self = this;
        this.setState({ settings, hasSavedState: savedState && savedMachine })
    }
    incrementLives(amount) {
        const lives = Math.max( Math.min( this.state["settings"]["lives"] + amount, 6 ), 3 )
        const settings = Object.assign({}, this.state.settings, { lives })
        this.setState({ settings }, () => {
            localForage.setItem("settings", this.state.settings)
        })
    }
    selectForeground() {
        this.setState({
            selectingForeground: true,
            tempForeground: this.state.settings.foreground
        })
    }
    selectBackground() {
        this.setState({
            selectingBackground: true,
            tempBackground: this.state.settings.background
        })
    }
    setForeground(color) {
        const settings = Object.assign({}, this.state.settings, {
            foreground: color
        })
        this.setState({ settings })
    }
    setBackground(color) {
        const settings = Object.assign({}, this.state.settings, {
            background: color
        })
        this.setState({ settings })
    }
    applyForeground() {
        this.setState({ selectingForeground: false }, () => {
            localForage.setItem("settings", this.state.settings)
        })
    }
    applyBackground(color) {
        this.setState({ selectingBackground: false }, () => {
            localForage.setItem("settings", this.state.settings)
        })
    }
    cancelForeground() {
        const settings = Object.assign({}, this.state.settings, {
            foreground: this.state.tempForeground
        })
        this.setState({ settings, selectingForeground: false })
    }
    cancelBackground() {
        const settings = Object.assign({}, this.state.settings, {
            background: this.state.tempBackground
        })
        this.setState({ settings, selectingBackground: false })
    }
    render() {
        const lives = this.state.settings.lives
        const hearts = Array.from({length: 6}, (value, i) => {
            return (<img src={heart} key={`h${i}`} className={i >= lives ? "disabled" : ""} />)
        })

        return (
            <div className={"home-screen"}>
                <div className={"header"}>
                    <img className={"logo"} src={logo} />
                    <div className={"title"}>
                        {"Space Invaders"}
                    </div>
                </div>
                <div className={"content"}>
                    <div className={"buttons"}>
                        <button className={"button"} onClick={() => this.props.onStart()}>
                            {"Start New Game"}
                        </button>
                        <button
                        className={"button"}
                        onClick={() => this.props.onStart(true)}
                        disabled={!this.state.hasSavedState}
                        >
                            {"Load Game"}
                        </button>
                    </div>
                    <div className={"settings-label"}>
                        {"Settings"}
                    </div>
                    <div className={"color-container"}>
                        <div className={"color-select"}>
                            <div>{"Foreground Color"}</div>
                            <div
                            className={"color-value"}
                            onClick={this.selectForeground.bind(this)}
                            style={{ backgroundColor: this.state.settings.foreground }}
                            >
                            </div>
                            <ColorSelector
                            title={"Select Foreground Color"}
                            isOpen={this.state.selectingForeground}
                            onApply={this.applyForeground.bind(this)}
                            onCancel={this.cancelForeground.bind(this)}
                            color={this.state.settings.foreground}
                            palette={palette}
                            onColorSet={this.setForeground.bind(this)}
                            />
                        </div>
                        <div className={"color-select"}>
                            <div>{"Background Color"}</div>
                            <div
                            className={"color-value"}
                            onClick={this.selectBackground.bind(this)}
                            style={{ backgroundColor: this.state.settings.background }}
                            >
                            </div>
                            <ColorSelector
                            title={"Select Background Color"}
                            isOpen={this.state.selectingBackground}
                            onApply={this.applyBackground.bind(this)}
                            onCancel={this.cancelBackground.bind(this)}
                            color={this.state.settings.background}
                            palette={palette}
                            onColorSet={this.setBackground.bind(this)}
                            />
                        </div>
                    </div>
                    {<div className={"lives-container"}>
                        <div className={"label"}>{"Lives:"}</div>
                        <button className={"decrement"} onClick={this.incrementLives.bind(this, -1)}>
                            {"-"}
                        </button>
                        <div className={"hearts"}>{hearts}</div>
                        <button className={"increment"} onClick={this.incrementLives.bind(this, 1)}>
                            {"+"}
                        </button>
                    </div>}
                </div>
            </div>
        )
    }
}

module.exports = HomeScreen
