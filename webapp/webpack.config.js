const path    = require('path')
const webpack = require('webpack')
const CleanWebpackPlugin    = require('clean-webpack-plugin')
const HtmlWebpackPlugin     = require('html-webpack-plugin')
const HtmlWebpackTemplate   = require('html-webpack-template')
const GoogleFontsPlugin     = require("google-fonts-webpack-plugin")
const FaviconsWebpackPlugin = require('favicons-webpack-plugin')
const ExtractTextPlugin     = require('extract-text-webpack-plugin')
const UglifyJSPlugin        = require('uglifyjs-webpack-plugin')
const theme                 = require('./theme.json')

const sass_prepend = [
    '@import "bourbon/bourbon";',
    `$render-width:  ${theme["render-width"]}px;`,
    `$render-height: ${theme["render-height"]}px;`,
    '@import "theme";',
    '@import "modules";'
].join('\n')

module.exports = {
    entry:  {
        app: './src/main.js',
        /*sw: './src/sw.js', */
    },
    output: { filename: '[name].js', path: path.resolve(__dirname, 'build') },
    node:   { fs: "empty" },
    plugins: [
        //new UglifyJSPlugin({ sourceMap: true }),
        new CleanWebpackPlugin(['build'], {
            exclude: [ "bin.wasm", "bin.data", "bin.js" ]
        }),
        new FaviconsWebpackPlugin({
            logo: path.resolve(__dirname, 'src', 'images', 'icon.png'),
            persistentCache: true,
            inject: true,
            background: '#FFF',
            title: 'Space Invaders',
            icons: {
                android: true,
                appleIcon: true,
                appleStartup: false,
                coast: false,
                favicons: true,
                firefox: false,
                windows: false,
                yandex: false
            }
        }),
        new HtmlWebpackPlugin({
            title: 'Space Invaders',
            inject: false,
            template: HtmlWebpackTemplate,
            appMountId: 'app',
            mobile: true,
            links: [ "https://fonts.googleapis.com/icon?family=Material+Icons" ],
            meta:  [
                { "apple-mobile-web-app-capable": "yes" },
                { "apple-mobile-web-app-status-bar-style": "black" },
                { "viewport": "user-scalable=no, width=device-width" }
            ]
        }),
        new ExtractTextPlugin("styles.css"),
        new GoogleFontsPlugin({
            fonts: theme["thirdPartyFonts"]["google"].map(f => ({ family: f }))
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
