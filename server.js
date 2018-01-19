const express = require('express')
const http    = require('http')
const path    = require('path')
const port    = 3020

// create app and server objects
var app    = express()
var server = http.createServer( app )

// serve front end static files
app.use( express.static( path.join( __dirname, "build" ) ) )

// start server
server.listen( port, () => console.log( 'listening on', port ) )
