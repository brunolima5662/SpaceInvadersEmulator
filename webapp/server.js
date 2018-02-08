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
            // set mime type to application/wasm
            response.header("Content-Type", "application/wasm")
        }
        else if(path.extname(_path) === ".appcache") {
            // set mime type to text/cache-manifest
            response.header("Content-Type", "text/cache-manifest")
        }
    }
}))

// start server
server.listen( port, () => console.log( 'listening on', port ) )
