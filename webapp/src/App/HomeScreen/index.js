import React from 'react'
import './home_screen.scss'

class HomeScreen extends React.Component {
    render() {
        return (
            <div className={"homescreen"}>
                <div className={"title"}>{"Space Invaders"}</div>
                <div className={"content"}></div>
            </div>
        )
    }
}

module.exports = HomeScreen
