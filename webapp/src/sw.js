importScripts('/node_modules/workbox-sw/build/importScripts/workbox-sw.prod.v2.1.2.js')

const workbox = new WorkboxSW({
    skipWaiting: true,
    clientsClaim: true
})

workbox.router.registerRoute(
    new RegExp('^.*$'),
    workbox.strategies.cacheOnly()
)

workboxSW.precache([]);
