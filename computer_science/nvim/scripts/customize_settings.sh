echo "-- Configuring ~/.local/share/nvim/archvim/opts.json"
mkdir -p ~/.local/share/nvim/archvim

YesOrNo() {
    if [ "x$2" = "xn" ]; then
        prompt="是或否，默认选否 [y/N] "
    else
        prompt="是或否，默认选是 [Y/n] "
    fi
    echo -n "$prompt"
    read -n1 answer < /dev/tty || read answer < /dev/tty || answer=$2
    if [ "x$2" = "xn" ]; then
        if [ "x$answer" != "xy" ]; then
            answer=0
        else
            answer=1
        fi
    else
        if [ "x$answer" != "xn" ]; then
            answer=1
        else
            answer=0
        fi
    fi
    return $answer
}

SetConfig() {
    echo "(设置中，请稍候...)"
    key=$1
    if [ $2 != 0 ]; then
        value=true
    else
        value=false
    fi
    nvim --headless -c "lua require'archvim.options'.$key = $value" -c 'sleep 1 | q!'
    echo
    return $2
}

cat << EOF
******************************************************
现在开始为您的个性化定制，如果不确定，可以一路按 ENTER 下去
******************************************************
EOF

cat << EOF
-----------------------------------------------------------------------------------------
If the symbol ' ' is not showing correctly, you might want to install Nerd Fonts.
此字符 “ ” 若无法显示，可能是因为您没有安装 Nerd Fonts 字体。

Nerd Fonts 官网下载：https://www.nerdfonts.com/
推荐选择 “JetBrainMono Nerd Fonts” 字体。
If you are using WSL or remote SSH, you'll have to manually download the font on Windows.
如果您使用的是 WSL 或者远程服务器连接，那么则需要为 Windows 客户端安装此字体。
Windows 终端 Nerd Fonts 安装教程：https://medium.com/@vedantkadam541/beautify-your-windows-terminal-using-nerd-fonts-and-oh-my-posh-4f4393f097
        
Worry not, without Nerd Fonts, you may still use NeoVim without the fancy icons.
别担心，即使没有安装此字体，您仍然可以正常使用 NeoVim，只不过没了一些花哨的图标。
-----------------------------------------------------------------------------------------
EOF

cat << EOF
==================================================================
Did your terminal shows this symbols correctly?  (a clock symbol)
您的终端是否能正常显示此字符？ （此处应为时钟符号）
==================================================================
EOF
YesOrNo n
SetConfig nerd_fonts $?

if [ $? != 0 ]; then
    cat << EOF
==================================================================
Did your terminal shows this symbols correctly?  (a cross symbol)
您的终端是否能正常显示此字符？（此处应为叉号）
==================================================================
EOF
    YesOrNo n
    SetConfig nerd_fonts $?
fi

cat << EOF
==================================================================
Do you need to use this NeoVim in remote connection (SSH)?'
您是否需要在远程连接（SSH）中使用此 NeoVim？'
==================================================================
EOF
YesOrNo y
SetConfig disable_notify $?

cat << EOF
==================================================================
Did you set any background image for the terminal?
您是否为终端设定了背景贴图（例如二次元壁纸）？
==================================================================
EOF
YesOrNo n
SetConfig transparent_color $?

cat << EOF
==================================================================
Inlay Hint:     add(a: 1, b: 2)"
No Inlay Hint:  add(1, 2)"
Inlay Hint:     auto i:int = 1;"
No Inlay Hint:  auto i = 1;"

Would you like to enable inlay hints?'
您是否希望开启 Inlay Hint 提示？'
==================================================================
EOF
YesOrNo n
SetConfig enable_inlay_hint $?

cat << EOF
==================================================================
Would you like to shared clipboard between NeoVim and system?
您是否希望 NeoVim 与系统共享剪贴板？
==================================================================
EOF
YesOrNo y
SetConfig enable_clipboard $?

cat << EOF
--
-- You may always change these settings later by re-running this script.
-- 您以后可以随时重新运行本脚本以修改这些上述设置。
-- 脚本路径：~/.config/nvim/scripts/customize_settings.sh
--
EOF
