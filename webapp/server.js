const express  = require('express')
const http     = require('http')
const path     = require('path')
const port     = 3020

// create app and server objects
var app    = express()
var server = http.createServer( app )

// serve front end static files
app.use(express.static(path.join( __dirname, "build" ), {
    setHeaders: (response, _path) => {
        if(path.extname(_path) === ".wasm") {
            // mime type to application/wasm
            response.header("Content-Type", "application/wasm")
        }
    }
}))

// start server
server.listen( port, () => console.log( 'listening on', port ) )
