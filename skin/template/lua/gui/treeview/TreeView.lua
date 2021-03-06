local TreeItem = require "gui.treeview.TreeItem"
local GcCallbacks = require "utility.GcCallbacks"
local TreeView = FLua.Class("TreeView")

local FolderItem = FLua.Class(TreeItem, "FolderItem")
do
    function FolderItem.new(xml, name)
        local obj = FolderItem()
        obj.name_ = name
        obj.node_xml = xml
        obj:data().folder_ = true
        return obj
    end
    
    function FolderItem:__constructor()
        self.name_ = ""
        self.checked_ = false
        self.userdata_ = nil
        self.path_ = ""
        theApp:GetRuntimeState():GetEventMgr():AddEvent(self, _G.Event.OnSearchFilter)
    end
    
    function FolderItem:__destructor()
        theApp:GetRuntimeState():GetEventMgr():DelEvent(self)
        warn("FolderItem:__destructor", self)
    end
    
    function FolderItem:toString()
        return string.format("[%s][%s] num_children:%d, folder:%s", self:GetPointer(), self.name_, self:num_children(), tostring(self:folder()))
    end

    function FolderItem:OnCreate()
        TreeItem.OnCreate(self)
        local pListElement = self:data().list_elment_
        local nameObj = pListElement:FindSubControl("tree_name")
        local checkObj = pListElement:FindSubControl("tree_expandbtn")
        local chooseObj = pListElement:FindSubControl("tree_choose")
        nameObj:SetTagCtrl(pListElement)
        checkObj:SetTagCtrl(pListElement)
        chooseObj:SetTagCtrl(pListElement)
        pListElement:OnNotifyAdd(function(msg)
            if msg.sType == "itemactivate" then
                if not self:data().child_visible_ then
                    self:Expand()
                else
                    self:Shrink()
                end
            end
        end)
        checkObj:OnNotifyAdd(function(msg)
            if msg.sType == "selectchanged" then
                if not self:data().child_visible_ then
                    self:Expand()
                else
                    self:Shrink()
                end
            end
        end)
        chooseObj:OnNotifyAdd(function(msg)
            if msg.sType == "selectchanged" then
                self:Choose(not self.checked_)
            end
        end)

        nameObj:SetText(self.name_)

        _G.coro.start(function()
            Utils.SearchPath(self:GetPath() .. self:GetName(), false, function(is_file, name, fullpath)
                if not is_file then
                    theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.AddTreeNode, self, true, name)
                    _G.coro.yield()
                elseif fullpath:endswith(".tmpl") then
                    local config_type_info = self:GetUserData()
                    local class_name = config_type_info.class_name
                    local msg_class = config_common:GetTmplClass(class_name)

                    local Tmpl = require "data.Tmpl"
                    local tmpl = Tmpl.new(fullpath, msg_class) 

                    theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.AddTreeNode, self, false, name, tmpl)
                    
                    _G.coro.yield()
                end
            end)

            -- local mb_root = helper.UTF8ToMB(root_dir)
            -- for x, _ in lfs.dir(mb_root .. "*.*") do
            --     if x ~= "." and x ~= ".."then
            --         local attr = lfs.attributes(mb_root .. x)
            --         Utils.printValue("x", helper.MBToUTF8(x), attr)
            --         if attr.mode == "directory" then
            --             local folder_name = helper.MBToUTF8(x)
            --             theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.AddTreeNode, self, true, folder_name)
            --             _G.coro.yield()
            --         elseif attr.mode == "file" then
            --             local file_name = helper.MBToUTF8(x)
            --             theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.AddTreeNode, self, false, file_name)
            --             _G.coro.yield()
            --         end
            --     end
            -- end
        end)
    end

    function FolderItem:Choose(b)
        self.checked_ = b
        for i=1, self:num_children() do
            local child = self:child(i)
            if child:is(FolderItem) then
                child:Choose(b)
                local chooseObj = child:data().list_elment_:FindSubControl("tree_choose")
                chooseObj:SetCheck(b)
            end
        end
        if not b then
            local parent = self:parent()
            while parent do
                if parent:is(FolderItem) then
                    local chooseObj = parent:data().list_elment_:FindSubControl("tree_choose")
                    chooseObj:SetCheck(b)

                    parent.checked_ = false 
                end

                parent = parent:parent()
            end
        end 
    end

    function FolderItem:SetUserData(u)
        self.userdata_ = u
    end
    function FolderItem:GetUserData()
        return self.userdata_
    end

    function FolderItem:GetName()
        return self.name_
    end
    function FolderItem:SetPath(p)
        self.path_ = p
    end
    function FolderItem:GetPath()
        return self.path_
    end

    function FolderItem:Expand(all)
        if self:CanExpand() then
            self:data().has_expand_ = true
            self:SetChildVisible(true)
        end
        if all then
            for i=1, self:num_children() do
                local child = self:child(i)
                if child:folder() then
                    child:Expand(all)
                end
            end
        end
    end

    function FolderItem:Shrink(all)
        if all then
            for i=1, self:num_children() do
                local child = self:child(i)
                if child:folder() then
                    child:Shrink(all)
                end
            end
        end
        if self:CanExpand() then
            self:SetChildVisible(false)
            self:data().has_expand_ = false
        end
    end

    function FolderItem:MatchFilter(txt)
        if txt ~= "" then
            if string.find(self:GetName(), txt) ~= nil then
                return true
            elseif self:has_children() then
                local n_count = 0
                for i=1, self:num_children() do
                    local child = self:child(i)
                    if child:MatchFilter(txt) then
                        n_count = n_count + 1
                    end
                end
                return n_count >0
            else
                return false
            end
        else
            return true
        end
    end

    function FolderItem:OnSearchFilter(txt)
        print("OnSearchFilter", txt, self)
        if self:MatchFilter(txt) then
            self:SetNodeVisible(true)
        else
            self:SetNodeVisible(false)
        end
    end
end
local TemplateItem = FLua.Class(TreeItem, "TemplateItem")
do
    local Tmpl = require "data.Tmpl"
    function TemplateItem.new(xml)
        local obj = TemplateItem()
        obj.node_xml = xml
        return obj
    end
    function TemplateItem:__constructor()
        self.userdata_ = nil
        self.tmpl_ = nil 
    end
    function TemplateItem:OnCreate()
        TreeItem.OnCreate(self)

        local pListElement = self:data().list_elment_
        local nameObj = pListElement:FindSubControl("tmpl_name")
        local idObj = pListElement:FindSubControl("tmpl_id")
        nameObj:SetTagCtrl(pListElement)
        idObj:SetTagCtrl(pListElement)

        nameObj:SetText(self.tmpl_:GetTmplName())

        pListElement:OnNotifyAdd(function(msg)
            if msg.sType == "itemactivate" then
                theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.OnOpenTmpl, self.tmpl_)
            end
        end)
    end

    function TemplateItem:SetUserData(u)
        self.userdata_ = u
    end
    function TemplateItem:GetUserData()
        return self.userdata_
    end

    function TemplateItem:SetTmpl(tmpl)
        self.tmpl_ = tmpl
    end
    function TemplateItem:GetTmpl()
        return self.tmpl_
    end

    function TemplateItem:MatchFilter(txt)
        if txt ~= "" then
            if string.find(self.tmpl_:GetTmplName(), txt) ~= nil then
                return true
            end

            return false
        else
            return true
        end 
    end
end


function TreeView.new(pCtrl)
    assert(pCtrl ~= nil and pCtrl:IsClass("List"), "pCtrl is nil")
    local obj = TreeView()
    obj._pTreeViewCtrl = pCtrl
    obj:OnInit()
	return obj
end
function TreeView:__constructor()
    self._pTreeViewCtrl = nil
    self.root_node_ = nil
	self.delay_deltaY_ = nil
	self.delay_number_ = nil 
	self.delay_left_ = nil
	self.text_padding_ = {left=0, top=0, right=0, bottom=0}
	self.level_text_start_pos_ = 8
end
function TreeView:__destructor()
   warn("TreeView:__destructor")
end

function TreeView:OnInit()
    self._pTreeViewCtrl:SetItemShowHtml(true)

    self.root_node_ = TreeItem()
	self.root_node_:data().level_ = 0
	self.root_node_:data().child_visible_ = true
    self.root_node_:data().has_expand_ = true
	self.root_node_:data().list_elment_ = nil
end

function TreeView:GetManager()
    return self._pTreeViewCtrl:GetManager()
end

function TreeView:GetRoot()
    return self.root_node_
end

function TreeView:GetLevelTextStartPos()
    return self.level_text_start_pos_
end

function TreeView:GetItemIndex(node)
    if node == self.root_node_ then return -1 end
    assert(self._pTreeViewCtrl)
    assert(node:data().list_elment_)
    return self._pTreeViewCtrl:GetItemIndex(node:data().list_elment_)
end 
function TreeView:GetTreeCtrl()
    return self._pTreeViewCtrl
end

function TreeView:Add(node, parent)
    if not parent then
        parent = self.root_node_
    end

    if not node:AddToTree(self, parent) then
        return nil
    end

    return node
end

function TreeView:RemoveNode(node)
    if not node or node == self.root_node_ then
        return false
    end
    for i=node:num_children(), 1, -1 do
		local child = node:child(i)
		self:RemoveNode(child)
    end

    self._pTreeViewCtrl:Remove(node:data().list_elment_)
	node:parent():remove_child(node)
    node = nil
    
	return true
end

function TreeView:SetChildVisible(node, visible)
    if not node or node == self.root_node_ then
        return
    end

    if node:data().child_visible_ == visible then
        return
    end

    node:data().child_visible_ = visible
    
    if node:data().has_child_ then
    end

    if visible and not node:data().list_elment_:IsVisible() then
        return
    end

    if not node:has_children() then
        return
    end

    for i=1, node:num_children() do
        local cnode = node:child(i)
        cnode:SetNodeVisible(visible)
    end
end

function TreeView:SetNodeVisible(node, visible)
    if not node or node == self.root_node_ then
        return
    end

    if node:parent() and node:parent() ~= self.root_node_ then
        if visible and not node:parent():IsExpand() then 
            return
        end
    end

    if visible ~= node:data().list_elment_:IsVisible() then
        node:data().list_elment_:SetVisible(visible)
    end

    if node:IsExpand() then
        self:SetChildVisible(node, visible)
    else
        self:SetChildVisible(node, false)
    end
end

function TreeView:CanExpand(node)
    if not node or node == self.root_node_ then
        return false
    end

	return node:data().has_child_
end

return TreeView