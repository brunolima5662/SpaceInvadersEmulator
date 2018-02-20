

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

function rgbFromHex( hex ) {
    const hex_digits = hex.replace(/\W/ig, '')
    const num_chars  = hex_digits.length / 3
    if( hex_digits.length == 3 || hex_digits.length == 6 ) {
        var rgb = [
            parseInt( hex_digits.substr( 0, num_chars ), 16 ), // red
            parseInt( hex_digits.substr( 1 * num_chars, num_chars ), 16 ), // green
            parseInt( hex_digits.substr( 2 * num_chars, num_chars ), 16 ), // blue
        ]
        if( num_chars == 1 ) {
            rgb = rgb.map( x => ( x << 4 ) | x )
        }
        return rgb
    }
    else {
        throw new Error( "Invalid color hex value" )
    }
}

// return an array in the format RGB332 [r, g, b] from a hex color value
export const rgb332FromHex = hex => {
    var rgb = rgbFromHex( hex )
    rgb[0]  = parseInt( Math.round ( rgb[0] / 36.0 ) )
    rgb[1]  = parseInt( Math.round ( rgb[1] / 36.0 ) )
    rgb[2]  = parseInt( Math.round ( rgb[2] / 85.0 ) )
    return rgb
}

// return either black or white, whichever has a better
// contrast with the given color
export const contrastColor = hex => {
    const rgb = rgbFromHex( hex ).map( x => x / 255.0 )
    const r = rgb[0] < 0.03928 ? ( rgb[0] / 12.92 ) : Math.pow( ( ( rgb[0] + 0.055 ) / 1.055 ), 2.4 )
    const g = rgb[1] < 0.03928 ? ( rgb[1] / 12.92 ) : Math.pow( ( ( rgb[1] + 0.055 ) / 1.055 ), 2.4 )
    const b = rgb[2] < 0.03928 ? ( rgb[2] / 12.92 ) : Math.pow( ( ( rgb[2] + 0.055 ) / 1.055 ), 2.4 )
    const l = 0.2126 * r + 0.7152 * g + 0.0722 * b
    return l > 0.179 ? '#000000' : '#ffffff'
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
    const rgb = rgb332FromHex( hex )
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

export default {
    cArray,
    jsArray,
    freeCArray,
    rgb332,
    rgb888,
    generateRGB332Palette,
    rgbFromHex,
    rgb332FromHex,
    contrastColor
}
