

// convert a javascript typed array into a c array
// by allocating and copying it into Emcripten's heap
// and returning a reference for it...
export const malloc = arr => {
    const bytes   = arr.length * arr.BYTES_PER_ELEMENT
    const pointer = Module._malloc( bytes )
    const c_arr   = new Uint8Array( Module.HEAPU8.buffer, pointer, bytes )
    c_arr.set( new Uint8Array( arr.buffer ) )
    return c_arr
}

// convert a single byte integer color value of format rgb332
// to a hex string representing the given color in the format rgb888
export const rgb888 = color => {
    const rgb = [
        parseInt( ( ( color >> 5 ) & 0x07 ) * 255.0 / 7.0 ).toString(16), // red
        parseInt( ( ( color >> 2 ) & 0x07 ) * 255.0 / 7.0 ).toString(16), // green
        parseInt( ( color & 0x03 ) * 255.0 / 3.0 ).toString(16) // blue
    ]

    return rgb.map(v => {
        return ( v.length == 1 ) ? `0${v}` : v
    }).reduce((hex, v) => {
        return hex + v
    }, '#')
}

// convert hex color value of format rgb888 to a single byte color
// value representing the given hex value in the format rgb332
export const rgb332 = hex => {
    const raw_hex = hex.replace(/\W/ig, '')
    const rgb = [
        parseInt( Math.round( parseInt( raw_hex.substr(0, 2), 16 ) / 36.0 ) ), // red
        parseInt( Math.round( parseInt( raw_hex.substr(2, 2), 16 ) / 36.0 ) ), // green
        parseInt( Math.round( parseInt( raw_hex.substr(4, 2), 16 ) / 85.0 ) ), // blue
    ]
    return ( ( rgb[0] << 5 ) | ( rgb[1] << 2 ) | rgb[2] )
}

export default { malloc, rgb332, rgb888 }
