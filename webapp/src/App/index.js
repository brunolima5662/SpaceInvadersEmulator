import React from 'react'
import RenderScreen from './RenderScreen'

import './app.scss'

class App extends React.Component {
    render () {
        return (
            <div id={"ui_container"}>
                <RenderScreen onCanvasReady={this.props.onCanvasReady} />
            </div>
        )
    }
}

module.exports = App
