import React from 'react'
import PropTypes from 'prop-types'
import Transition from 'react-transition-group/Transition'

const Snackbar = ({ display, style, className, children }) => {
    const sass = require('./styles.scss')
    const states = {
        entering: { marginBottom: `-${sass['height']}`, },
        exited:   { marginBottom: `-${sass['height']}`, },
        entered:  { marginBottom: '0px', transitionTimingFunction: 'ease-in' },
        exiting:  { marginBottom: '0px', transitionTimingFunction: 'ease-in' }
    }
    console.log(states)
    return (
        <Transition in={display} timeout={300}>
            {state => (
                <div className={`${sass['id']} ${className}`} style={{...style, ...states[state]}}>
                    {children}
                </div>
            )}
        </Transition>
    )
}

Snackbar.propTypes = {
    display: PropTypes.bool.isRequired,
    style: PropTypes.object,
    className: PropTypes.string,
    children: PropTypes.any.isRequired
}

export default Snackbar
