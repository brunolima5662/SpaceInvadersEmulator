import React from 'react'
import './home_screen.scss'
import logo from './logo.png'

class HomeScreen extends React.Component {
    render() {
        return (
            <div className={"homescreen"}>
                <div className={"header"}>
                    <img className={"logo"} src={logo} />
                    <div className={"title"}>
                        {"Space Invaders"}
                    </div>
                </div>
                <div className={"content"}>
                    <button className={"start"}>{"Start Game"}</button>
                </div>
            </div>
        )
    }
}

module.exports = HomeScreen
