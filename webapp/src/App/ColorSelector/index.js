import React from 'react'
import PropTypes from 'prop-types'
import Modal from '../Modal'
import './styles.scss'

const defaultPalette = [
    "#f44336", "#e91e63", "#9c27b0", "#673ab7", "#3f51b5", "#2196f3",
    "#03a9f4", "#00bcd4", "#009688", "#4caf50", "#8bc34a", "#cddc39",
    "#ffeb3b", "#ffc107", "#ff9800", "#ff5722", "#795548", "#607d8b"
]

const ColorSelector = ({ title, isOpen, onApply, onCancel, color, palette, onColorSet }) => (
    <Modal
    open={isOpen}
    onClose={onCancel}
    title={( <div className={"color-picker-title"}> {title} </div> )}
    actions={[
        (
            <button className={"color-picker-cancel"} onClick={onCancel}>
                {"Cancel"}
            </button>
        ),
        (
            <button className={"color-picker-apply"} onClick={onApply}>
                {"Apply"}
            </button>
        )
    ]}>
        <div className={"color-picker-container"}>
            <div className={"current"} style={{backgroundColor: color}}>
                {color}
            </div>
            <div className={"swatches"}>
                {(palette || defaultPalette).map(c => {
                    return (
                        <div
                        key={c}
                        className={`swatch ${c == color ? 'selected' : ''}`}
                        onClick={onColorSet.bind(null, c)}
                        style={{backgroundColor: c}}
                        >
                        </div>
                    )
                })}
            </div>

        </div>
    </Modal>
)

ColorSelector.propTypes = {
    title: PropTypes.string.isRequired,
    isOpen: PropTypes.bool.isRequired,
    onApply: PropTypes.func.isRequired,
    onCancel: PropTypes.func.isRequired,
    color: PropTypes.string.isRequired,
    palette: PropTypes.array,
    onColorSet: PropTypes.func.isRequired
}

export default ColorSelector
