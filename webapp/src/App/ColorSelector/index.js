import React from 'react'
import { CirclePicker } from 'react-color'
import Dialog from 'material-ui/Dialog'
import FlatButton from 'material-ui/FlatButton'

const ColorSelector = ({ type, isOpen, onApply, onCancel, color, onColorSet }) => (
    <Dialog
    title={`Select ${ type } Color`}
    modal={true}
    open={isOpen}
    actions={[
        <FlatButton label="Cancel" primary={true} onClick={onCancel}  />,
        <FlatButton label="Apply" primary={true} onClick={onApply}  />
    ]}>
        <CirclePicker
        color={ color }
        onChangeComplete={ onColorSet }
        />
    </Dialog>
)

export default ColorSelector
