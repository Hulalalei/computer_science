-- https://github.com/microsoft/vscode/blob/main/src/vs/base/common/codicons.ts
-- go to the above and then enter <c-v>u<unicode> and the symbold should appear
-- or go here and upload the font file: https://mathew-kurian.github.io/CharacterMap/
-- find more here: https://www.nerdfonts.com/cheat-sheet

local icons = {
    kind = {
        Text = "󰦨 ",
        Method = " ",
        Function = " ",
        Constructor = " ",
        Field = " ",
        Variable = " ",
        Class = " ",
        Interface = " ",
        Module = " ",
        Property = " ",
        Unit = " ",
        Value = "󰾡 ",
        Enum = " ",
        Keyword = " ",
        Snippet = " ",
        Color = " ",
        File = " ",
        Reference = " ",
        Folder = " ",
        EnumMember = " ",
        Constant = " ",
        Struct = " ",
        Event = " ",
        Operator = " ",
        TypeParameter = " ",
        Specifier = " ",
        Statement = "",
        Recovery = " ",
        TranslationUnit = " ",
        PackExpansion = " "
    },
    type = {
        Array = " ",
        Number = " ",
        String = " ",
        Boolean = " ",
        Object = " ",
        Template = " "
    },
    documents = {
        File = "",
        Files = "",
        Folder = "",
        OpenFolder = "",
        EmptyFolder = "",
        EmptyOpenFolder = "",
        Unknown = "",
        Symlink = "",
        FolderSymlink = ""
    },
    git = {
        Add = " ",
        Mod = " ",
        Remove = " ",
        Untrack = " ",
        Rename = " ",
        Diff = " ",
        Repo = " ",
        Branch = " ",
        Unmerged = " "
    },
    ui = {
        Lock              = "",
        TinyCircle        = "",
        Circle            = "",
        BigCircle         = "",
        BigUnfilledCircle = "",
        CircleWithGap     = "",
        LogPoint          = "",
        Close             = "",
        NewFile           = "",
        Search            = "",
        Lightbulb         = "",
        Project           = "",
        Dashboard         = "",
        History           = "",
        Comment           = "",
        Bug               = "",
        Code              = "",
        Telescope         = " ",
        Gear              = "",
        Package           = "",
        List              = "",
        SignIn            = "",
        Check             = "",
        Fire              = "",
        Note              = "",
        BookMark          = "",
        Pencil            = "",
        ChevronRight      = "",
        Table             = "",
        Calendar          = "",
        Line              = "▊",
        Evil              = "",
        Debug             = "",
        Run               = "",
        VirtualPrefix     = "",
        Next              = "",
        Previous          = "",
        Clock             = "",
    },
    diagnostics = {
        Error = " ",
        Warning = " ",
        Information = " ",
        Question = " ",
        Hint = " ",
    },
    misc = {
        Robot = "󰚩 ",
        Squirrel = "  ",
        Tag = " ",
        Arch = "󰣇 ",
    },
    cmake = {
        CMake = "",
        Build = "",
        Run = "",
        Debug = "",
    },
}

--- 如果icon文件不存在，这使用当前文件中的符号 ---
if not require'archvim.options'.nerd_fonts then
    for k, v in pairs(icons) do
        for k1, v1 in pairs(v) do
            icons[k][k1] = k1
        end
    end
end

return icons
