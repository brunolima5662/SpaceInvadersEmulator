

// convert a javascript typed array into a c array
// by allocating and copying it into Emcripten's heap
// and returning a reference for it...
export const cArray = arr => {
    const bytes     = arr.length * arr.BYTES_PER_ELEMENT
    const pointer   = Module._malloc( bytes )
    Module.HEAPU8.set( arr, pointer )
    return pointer
}

// convert a c array into a javascript UInt8Array by reading
// from emscripten at the given pointer with the given bytes
export const jsArray = (pointer, bytes) => {
    return new Uint8Array( Module.HEAPU8.buffer, pointer, bytes )
}

// free a typed array referencing a block of emscripten's heap
export const freeCArray = pointer => {
    Module._free(pointer.byteOffset)
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
    }).reduce(( hex, v ) => ( hex + v ), '#')
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

// generate 38-color rgb332 palette in the format rgb888
export const generateRGB332Palette = () => {
    const shades = Array.from({ length: 2 }, (v, shade) => {
        const shade_index = parseInt( Math.round( ( ( shade + 1.0 ) / 4.0 * 7.0 ) ) )
        return [
            rgb888( shade_index << 5 ), // red
            rgb888( shade_index << 2 ), // green
            rgb888( shade + 1 ), // blue
            rgb888( shade_index << 5 | ( shade + 1 ) ), // magenta
            rgb888( shade_index << 2 | ( shade + 1 ) ), // cyan
            rgb888( shade_index << 5 | shade_index << 2  ), // yellow
        ]
    })
    const tints = Array.from({ length: 3 }, (v, tint) => {
        const tint_index = parseInt( Math.round( ( tint / 4.0 * 7.0 ) ) )
        return [
            rgb888( 0xe0 | tint_index << 2 | tint ), // red
            rgb888( 0x1c | tint_index << 5 | tint ), // green
            rgb888( 0x03 | tint_index << 5 | tint_index << 2 ), // blue
            rgb888( 0xe3 | tint_index << 2 ), // magenta
            rgb888( 0x1f | tint_index << 5 ), // cyan
            rgb888( 0xfc | tint ), // yellow
        ]
    })
    return [ '#000000',
        ...shades[0], ...shades[1], ...tints[0], ...tints[1], ...tints[2],
    '#ffffff' ]
}

export default { cArray, jsArray, freeCArray, rgb332, rgb888, generateRGB332Palette }
