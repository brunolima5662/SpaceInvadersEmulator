import React from 'react'
import PropTypes from 'prop-types'

const styles = {
    overlay: {
        position: 'fixed', top: 0, bottom: 0, left: 0, right: 0,
        width: '100%',
        height: '100%',
        backgroundColor: 'rgba(0, 0, 0, 0.35)',
        justifyContent: 'center',
        alignItems: 'center'
    },
    modal: {
        maxWidth: '80%',
        maxHeight: '80%',
        minHeight: '20%',
        minWidth: '20%',
        backgroundColor: 'white',
        borderRadius: '4px',
        padding: '15px',
        boxShadow: '0px 0px 8px rgba(0, 0, 0, 0.12)',
        display: 'grid',
        gridTemplateColumns: 'auto',
        gridTemplateRows: '30px 1fr 30px'
    },
    title: {
        gridColumn: '1 / 2',
        gridRow: '1 / 2',
        display: 'flex',
        justifyContent: 'flex-start',
        textAlign: 'left',
        color: 'black'
    },
    content: {
        gridColumn: '1 / 2',
        gridRow: '2 / 3',
    },
    footer: {
        gridColumn: '1 / 2',
        gridRow: '3 / 4',
        display: 'flex',
        justifyContent: 'flex-end'
    }
}

const Modal = ({open, onClose, style, title, actions, children}) => (
    <div style={{ ...styles["overlay"], display: open ? 'flex' : 'none' }} onClick={onClose}>
        <div style={{ ...styles["modal"], style }} onClick={ev => ev.stopPropagation()}>
            {title && ( <div style={styles["title"]}> {title} </div> )}
            <div style={styles["content"]}> {children} </div>
            {actions && ( <div style={styles["footer"]}> {actions} </div> )}
        </div>
    </div>
)

Modal.propTypes = {
    open: PropTypes.bool.isRequired,
    onClose: PropTypes.func,
    style: PropTypes.object,
    title: PropTypes.oneOfType([ PropTypes.string, PropTypes.element ]),
    actions: PropTypes.oneOfType([ PropTypes.array, PropTypes.element ]),
    children: PropTypes.any
}

export default Modal
