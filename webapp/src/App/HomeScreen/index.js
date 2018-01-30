import React from 'react'
import './home_screen.scss'
import logo from './logo.png'
import heart from './heart.png'

class HomeScreen extends React.Component {

    render() {
        const lives = 3
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
                        <button className={"button"}>
                            {"Start New Game"}
                        </button>
                        <button className={"button"} disabled>
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
                        <button className={"decrement"}>{"-"}</button>
                        <div className={"hearts"}>{hearts}</div>
                        <button className={"increment"}>{"+"}</button>
                    </div>
                </div>
            </div>
        )
    }
}

module.exports = HomeScreen
