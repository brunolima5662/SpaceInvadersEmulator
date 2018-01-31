import React from 'react'
import localForage from 'localforage'
import './home_screen.scss'
import logo from './logo.png'
import heart from './heart.png'

class HomeScreen extends React.Component {
    constructor(props) {
        super(props)
        Promise.all([
            localForage.getItem("settings"),
            localForage.getItem("saved_state")
        ]).then(this.loadSettings.bind(this))
    }
    dataLoaded({ settings, savedState }) {
        this.setState({ settings, hasSavedState: savedState ? true : false })
    }
    incrementLives(amount) {
        const state = this.state
        state["settings"]["lives"] += amount
        this.setState(state)
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
                        <button className={"button"} onClick={this.props.onStart}>
                            {"Start New Game"}
                        </button>
                        <button className={"button"} disabled={!this.state.hasSavedState}>
                            {"Load Game"}
                        </button>
                    </div>
                    <div className={"settings-label"}>
                        {"Settings"}
                    </div>
                    <div className={"color-container"}>
                        <div className={"color-select"}>
                            <div>{"Foreground Color"}</div>
                            <div className={"color-value"} style={{backgroundColor: "#FFF"}}>
                            </div>
                        </div>
                        <div className={"color-select"}>
                            <div>{"Background Color"}</div>
                            <div className={"color-value"} style={{backgroundColor: "#000"}}>
                            </div>
                        </div>
                    </div>
                    <div className={"lives-container"}>
                        <div className={"label"}>{"Lives:"}</div>
                        <button className={"decrement"} onClick={this.incrementLives.bind(this, 1)}>
                            {"-"}
                        </button>
                        <div className={"hearts"}>{hearts}</div>
                        <button className={"increment"} onClick={this.incrementLives.bind(this, -1)}>
                            {"+"}
                        </button>
                    </div>
                </div>
            </div>
        )
    }
}

module.exports = HomeScreen
