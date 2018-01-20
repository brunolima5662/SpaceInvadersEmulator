import React from 'react'
import { render } from 'react-dom'
import injectTapEventPlugin from 'react-tap-event-plugin'

import App from './App'

// Render!
render( <div><App /></div> , document.getElementById('app') );
