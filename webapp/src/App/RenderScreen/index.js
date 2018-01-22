import React from 'react'

import './render_screen.scss'
const theme = require('../../../theme.json')

class RenderScreen extends React.Component {
    render() {
        return (
            <canvas
                ref={this.props.onCanvasReady}
                height={theme["render-height"]}
                width={theme["render-width"]}>
            </canvas>
        )
    }
}

module.exports = RenderScreen
