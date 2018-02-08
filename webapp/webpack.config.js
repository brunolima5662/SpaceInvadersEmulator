const path    = require('path')
const webpack = require('webpack')
const CleanWebpackPlugin    = require('clean-webpack-plugin')
const HtmlWebpackPlugin     = require('html-webpack-plugin')
const HtmlWebpackTemplate   = require('html-webpack-template')
const GoogleFontsPlugin     = require("google-fonts-webpack-plugin")
const FaviconsWebpackPlugin = require('favicons-webpack-plugin')
const ExtractTextPlugin     = require('extract-text-webpack-plugin')
const WorkboxPlugin         = require('workbox-webpack-plugin')
const AppCachePlugin        = require('appcache-webpack-plugin')
const UglifyJSPlugin        = require('uglifyjs-webpack-plugin')
const CopyWebpackPlugin     = require('copy-webpack-plugin')
const theme                 = require('./theme.json')

const sass_prepend = [
    '@import "bourbon/bourbon";',
    `$render-width:  ${theme["render-width"]}px;`,
    `$render-height: ${theme["render-height"]}px;`,
    '@import "theme";',
    '@import "modules";'
].join('\n')

const icons = `
    <link rel="apple-touch-icon" sizes="180x180" href="icons/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="icons/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="192x192" href="icons/android-chrome-192x192.png">
    <link rel="icon" type="image/png" sizes="16x16" href="icons/favicon-16x16.png">
    <link rel="manifest" href="site.webmanifest">
    <link rel="mask-icon" href="icons/safari-pinned-tab.svg" color="#55d555">
`

module.exports = {
    entry:  { app: './src/main.js' },
    output: { filename: '[name].js', path: path.resolve(__dirname, 'build') },
    node:   { fs: "empty" },
    plugins: [
        new UglifyJSPlugin({ sourceMap: true }),
        new CleanWebpackPlugin(['build'], {
            exclude: [ "bin.wasm", "bin.data", "bin.js" ]
        }),
        /*new FaviconsWebpackPlugin({
            logo: path.resolve(__dirname, 'src', 'images', 'icon.png'),
            inject: true,
            title: 'Space Invaders',
            persistentCache: true,
            icons: {
                android: { background: false },
                appleIcon: { background: '#181818' },
                appleStartup: false,
                coast: false,
                favicons: true,
                firefox: false,
                windows: false,
                yandex: false
            }
        }),*/
        new ExtractTextPlugin("styles.css"),
        new GoogleFontsPlugin({
            fonts: theme["thirdPartyFonts"]["google"].map(f => ({ family: f }))
        }),
        new WorkboxPlugin({
            globDirectory: path.resolve(__dirname, 'build'),
            globPatterns: ['**/*.{html,js,css,xml,wasm,data,eot,svg,ttf,woff,woff2,png,ico,webmanifest}'],
            runtimeCaching: [{
                urlPattern: /https:\/\/fonts.googleapis.com\/(.*)/,
                cacheName: 'googleapis',
                handler: 'cacheFirst',
                options: { cacheableResponse: { statuses: [ 0, 200 ] } }
            },{
                urlPattern: /(?:workbox-sw\.prod\.v.*\.js)(?:\.map)?$/,
                cacheName: 'workbox',
                handler: 'cacheFirst',
                options: { cacheableResponse: { statuses: [ 0, 200 ] } }
            },{
                urlPattern: /^.*$/,
                cacheName: 'mainapp',
                handler: 'cacheFirst'
            }]
        }),
        new CopyWebpackPlugin([{
            from: 'src/images/icons/*',
            to: 'icons/',
            ignore: [ '*.xml', '*.webmanifest', '*.ico' ],
            flatten: true
        },{
            from: 'src/images/icons/*',
            to: './',
            ignore: [ '*.svg', '*.png' ],
            flatten: true
        }]),
        new AppCachePlugin({
            output: 'manifest.appcache',
            cache: ['bin.js', 'bin.wasm', 'bin.data', 'index.html' ],
            exclude: [ 'theme.json', /^src.*$/, /\.appcache/ ]
        }),
        new HtmlWebpackPlugin({
            title: 'Space Invaders',
            inject: false,
            template: HtmlWebpackTemplate,
            appMountId: 'app',
            mobile: true,
            persistentCache: true,
            links: [ "https://fonts.googleapis.com/icon?family=Material+Icons" ],
            meta:  [
                { name: "apple-mobile-web-app-title", content: "Space Invaders" },
                { name: "application-name", content:  "Space Invaders" },
                { name: "apple-mobile-web-app-capable", content:  "yes" },
                { name: "apple-mobile-web-app-status-bar-style", content:  "black" },
                { name: "viewport", content:  "user-scalable=no, width=device-width" },
                { name: "msapplication-TileColor", content:  "#55d555" },
                { name: "theme-color", content:  "#55d555" }
            ],
            headHtmlSnippet: icons
        })

    ],
    module: {
        rules: [
            {
                test: /\.(.css|.scss)$/,
                use: ExtractTextPlugin.extract({
                    fallback: 'style-loader',
                    use: ['css-loader', {
                        loader: 'sass-loader',
                        options: {
                            data: sass_prepend,
                            includePaths: ["src/sass-helpers"]
                        }
                    }]
                })
            },
            {
                test: /\.(jpe?g|png|gif|svg|eot|woff|ttf|svg|woff2)$/,
                exclude: /(src\/images)/,
                use: {
                    loader: 'url-loader',
                    options: { name: '[name].[ext]' }
                }
            },
            {
                test: /\.(js|jsx)$/,
                exclude: /(node_modules|bower_components)/,
                use: {
                    loader: 'babel-loader',
                    options: {
                        sourceMap: true,
                        presets: [
                            'babel-preset-env',
                            'babel-preset-react',
                            'babel-preset-stage-0'
                        ]
                    }
                }
            }
        ]
    }
}
