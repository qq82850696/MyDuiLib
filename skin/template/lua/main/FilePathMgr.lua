local CFilePathMgr = FLua.Class("CFilePathMgr")

function CFilePathMgr:__constructor()
	self.m_sTemplateDatabasePath = ""
	self.m_sDatabasePath = ""
	self.m_sOnlineResourcePath = ""
	self.m_sDefaultCachePath = ""
	self.m_sImageCachePath = ""
	self.m_sExpressionPath = ""
	self.m_sLrcFilePath = ""
	self.m_sWebServiceCatchPath = ""
	self.m_sLogPath = ""

	self.m_sTemplateDatabasePath = self:GetAppPath()
	self.m_sDatabasePath = self:GetDataPath() .. "db/"
	--m_sExpressionPath = GetAppPath()+ _T("gif/");
	--em_utility::misc::confirm_dir(m_sExpressionPath,_T('/'));
	self.m_sDefaultCachePath = self:GetDataPath() .. "cache/"
	self.m_sLogPath = self:GetDataPath() .. "log/"
	--tmpl
	self.m_TmpRootPath = self:GetAppPath() .. "tmpl/"
end

function CFilePathMgr:Init()
	assert(Application.CreateDirectory(self:GetTmplRootPath()))
end

function CFilePathMgr:LinkAvatarPath(nUserID)
	local sImgPath = string.format("avater_%u.jpg", nUserID)
	sImgPath = self:GetImageCachePath() .. sImgPath
	return sImgPath
end
function CFilePathMgr:LinkListenPath( nResID, sFileName)
	local sFilePath = string.format("%u", nResID)
	sFilePath = self:GetOnlineResourcePath() .. sFilePath
	return sFilePath
end
function CFilePathMgr:LinkAlbumCoverPath(nSongID)
	local sImgPath = string.format("AlbumCover_%u.jpg", nSongID)
	sImgPath = self:GetImageCachePath() .. sImgPath
	return sImgPath
end

function CFilePathMgr:GetDatabasePath()
	return self.m_sDatabasePath
end
function CFilePathMgr:GetTemplateDataBasePath()
	return self.m_sTemplateDatabasePath
end
function CFilePathMgr:GetOnlineResourcePath()
	return ""
end
function CFilePathMgr:GetImageCachePath()
	return ""
end
function CFilePathMgr:GetDefaultCachePath()
	return self.m_sDefaultCachePath
end
function CFilePathMgr:GetCurrentCachePath()
end
-- GetExpressionPath();
function CFilePathMgr:GetLrcFilePath()
end
function CFilePathMgr:GetAppPath()
	local tempPath = Application.GetModulePath()
	tempPath = tempPath:gsub("\\", "/")
	if tempPath:sub(-1,-1) ~= "/" then
		tempPath = tempPath .. "/"
	end
	return tempPath
end
function CFilePathMgr:GetDataPath()
	local tempPath = Application.GetTempPath()
	tempPath = tempPath:gsub("\\", "/")
	if tempPath:sub(-1,-1) ~= "/" then
		tempPath = tempPath .. "/"
	end 
	return tempPath .. APP_NAME .. "/"
end
function CFilePathMgr:GetWebServiceCatchPath()
end
function CFilePathMgr:GetLogPath()
	return self.m_sLogPath
end

function CFilePathMgr:GetTmplRootPath()
	return self.m_TmpRootPath
end

return CFilePathMgr