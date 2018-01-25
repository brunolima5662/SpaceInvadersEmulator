

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

export default { malloc }
