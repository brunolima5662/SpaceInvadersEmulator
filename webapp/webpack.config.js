const path    = require('path')
const webpack = require('webpack')
const CleanWebpackPlugin    = require('clean-webpack-plugin')
const HtmlWebpackPlugin     = require('html-webpack-plugin')
const HtmlWebpackTemplate   = require('html-webpack-template')
const FaviconsWebpackPlugin = require('favicons-webpack-plugin')
const ExtractTextPlugin     = require('extract-text-webpack-plugin')
const UglifyJSPlugin        = require('uglifyjs-webpack-plugin')
const theme                 = require('./theme.json')

const sass_prepend = [
    `$render-width:  ${theme["render-width"]}px;`,
    `$render-height: ${theme["render-height"]}px;`,
].join('\n')

module.exports = {
    entry:  { app: './src/main.js', /*sw: './src/sw.js' */ },
    output: { filename: '[name].js', path: path.resolve(__dirname, 'build') },
    node:   { fs: "empty" },
    plugins: [
        new UglifyJSPlugin({ sourceMap: true }),
        new CleanWebpackPlugin(['build'], {
            exclude: [ "bin.wasm", "bin.data", "bin.js" ]
        }),
        new FaviconsWebpackPlugin({
            logo: path.resolve(__dirname, 'src', 'images', 'icon.png'),
            persistentCache: true,
            inject: true,
            background: '#FFF',
            title: 'Space Invaders',
            icons: { android: true, appleIcon: true, coast: false, favicons: true, firefox: false, windows: false }
        }),
        new HtmlWebpackPlugin({
            title: 'Space Invaders',
            inject: false,
            template: HtmlWebpackTemplate,
            appMountId: 'app',
            mobile: true,
            meta: [
                { "apple-mobile-web-app-capable": "yes" },
                { "apple-mobile-web-app-status-bar-style": "black" },
                { "viewport": "user-scalable=no, width=device-width" }
            ]
        }),
        new ExtractTextPlugin("styles.css")
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
            { test: /\.(png|svg|jpg)$/, use: [ 'file-loader' ] },
            {
                test: /\.(js|jsx)$/,
                exclude: /(node_modules|bower_components)/,
                use: {
                    loader: 'babel-loader',
                    options: {
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
