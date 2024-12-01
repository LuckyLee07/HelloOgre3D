GUI = {
	Fonts = {
		Small =           "small",
		SmallMono =       "small_mono",
		Medium =          "medium",
		MediumMono =      "medium_mono",
		Large =           "large",
		LargeMono =       "large_mono",
	},
	Markup = {
		Small =          "%@9%",
		SmallMono =      "%@91%",
		Medium =         "%@14%",
		MediumMono =     "%@141%",
		Large =          "%@24%",
		LargeMono =      "%@241%",
	},
	MarkupNewline = "\n",
	MarkupColor = {
		White =     "%0",
		Red =       "%1",
		Green =     "%2",
		Blue =      "%3",
		Yellow =    "%4",
		Purple =    "%5",
		Teal =      "%6",
		Orange =    "%7",
		Gray =      "%8",
		Black =     "%9",
	},
	MarkupColorTable = {
		[0] =    ColourValue(1.0, 1.0, 1.0, 1.0),
		[1] =    ColourValue(1.0,  0.0,  0.0,  1.0),
		[2] =    ColourValue(0.0,  1.0,  0.0,  1.0),
		[3] =    ColourValue(0.0,  0.0,  1.0,  1.0),
		[4] =    ColourValue(1.0,  1.0,  0.0,  1.0),
		[5] =    ColourValue(1.0,  0.0,  1.0,  1.0),
		[6] =    ColourValue(0.0,  1.0,  1.0,  1.0),
		[7] =    ColourValue(1.0,  0.65, 0.0,  1.0),
		[8] =    ColourValue(0.75, 0.75, 0.75, 1.0),
		[9] =    ColourValue(0.0,  0.0,  0.0,  1.0),
	},
	Palette = {
		DarkBlueGradient  = ColourValue(0.0, 0.2, 0.4, 0.5),
		DarkBlackGradient = ColourValue(0.1, 0.1, 0.1, 0.6),
	},
};


local cameraInfoPanel = nil
local profileInfoPanel = nil


local __averageRenderTimes__ = {}
local __averageSimTimes__ = {}
local __averageTotalSimTimes__ = {}

local __ComputerNewTime__ = function(times, newTime)
	table.insert(times, newTime)

	if #times > 15 then table.remove(times, 1) end

	local averageTime = 0;
	for index = 1, #times do
		averageTime = averageTime + times[index]
	end
	return averageTime / #times;
end

local __ColorValue__ = function(value, mediumValue, lowValue)
    if (value <= lowValue) then
        return GUI.MarkupColor.Red;
    elseif (value <= mediumValue) then
        return GUI.MarkupColor.Orange;
    end
   
    return GUI.MarkupColor.Green;
end


function GUI_CreateCameraInfo()
	local ui_width, ui_height = 300, 150;
    local screenWidth = GameManager:getScreenWidth()
    local screenHeight = GameManager:getScreenHeight()

    local uiComponent = Sandbox:CreateUIComponent()
    local ui_posx = screenWidth - ui_width - 20;
    local ui_posy = 35 + ui_height;
    uiComponent:setPosition(Vector2(ui_posx, ui_posy))
    uiComponent:setDimension(Vector2(ui_width, ui_height))
    uiComponent:setTextMargin(10, 10);
    uiComponent:setVisible(false)

	local startColor = GUI.Palette.DarkBlueGradient
    local endenColor = GUI.Palette.DarkBlackGradient
    uiComponent:setGradientColor(Gorilla.Gradient_NorthSouth, startColor, endenColor)

    return uiComponent
end


function GUI_CreateProfileInfo()
	local ui_width, ui_height = 300, 125;
    local screenWidth = GameManager:getScreenWidth()
    local screenHeight = GameManager:getScreenHeight()

    local uiComponent = Sandbox:CreateUIComponent()
    local ui_posx = screenWidth - ui_width - 20;
    uiComponent:setPosition(Vector2(ui_posx, 20))
    uiComponent:setDimension(Vector2(ui_width, ui_height))
    uiComponent:setTextMargin(10, 10);
    uiComponent:setVisible(false)

	local startColor = GUI.Palette.DarkBlueGradient
    local endenColor = GUI.Palette.DarkBlackGradient
    uiComponent:setGradientColor(Gorilla.Gradient_NorthSouth, startColor, endenColor)

    return uiComponent
end


function GUI_UpdateCameraInfo()
	if not cameraInfoPanel or not cameraInfoPanel:isVisible() then
	 	return 
	end
	
	local forward = Sandbox:GetCameraForward()
	local rotation = Sandbox:GetCameraRotation()
	local position = Sandbox:GetCameraPosition()
	local left = Sandbox:GetCameraLeft()
	local up = Sandbox:GetCameraUp()

	local cameraInfo = GUI.Markup.SmallMono ..
    "Camera Information:" .. GUI.MarkupNewline .. GUI.MarkupNewline ..
    string.format("  Position: |%7.2f, %7.2f, %7.2f |", position.x, position.y, position.z) ..
    GUI.MarkupNewline ..
    string.format("  Rotation: |%7.2f, %7.2f, %7.2f |", rotation.x, rotation.y, rotation.z) ..
    GUI.MarkupNewline ..
    GUI.MarkupNewline ..
    string.format("  Left:     |%7.2f, %7.2f, %7.2f |", left.x, left.y, left.z) ..
    GUI.MarkupNewline ..
    string.format("  Up:       |%7.2f, %7.2f, %7.2f |", up.x, up.y, up.z) ..
    GUI.MarkupNewline ..
    string.format("  Forward:  |%7.2f, %7.2f, %7.2f |", forward.x, forward.y, forward.z);

    cameraInfoPanel:setMarkupText(GUI.MarkupColor.White .. cameraInfo)
end

function GUI_UpdateProfileInfo()
	if not profileInfoPanel or not profileInfoPanel:isVisible() then
	 	return 
	end

	local renderTime = Sandbox:GetRenderTime() / 1000
	local avgRenderTime = __ComputerNewTime__(__averageRenderTimes__, renderTime);

	local simulateTime = Sandbox:GetSimulateTime() / 1000
	local avgSimulateTime = __ComputerNewTime__(__averageSimTimes__, simulateTime);

	local totalSimTime = Sandbox:GetTotalSimulateTime() / 1000
	local avgTotalSimTime = __ComputerNewTime__(__averageTotalSimTimes__, totalSimTime);

	local fps = 1000 / avgRenderTime
	local simSteps = 1000 / avgTotalSimTime

    local box_count = Shoot_BoxCount and Shoot_BoxCount or 0
	local profileInfo = GUI.Markup.SmallMono ..
    "Performance Information: -> " .. box_count .. GUI.MarkupNewline ..
    GUI.MarkupNewline ..    
    string.format(
        "  Frames per Second:        %s%7.0f%s",
        __ColorValue__(fps, 45, 25),
        fps,
        GUI.MarkupColor.White) ..
    GUI.MarkupNewline ..
    string.format("  Render Time:              %7.2f ms", avgRenderTime) ..
    GUI.MarkupNewline ..
    GUI.MarkupNewline ..
    string.format(
        "  Steps per Second:         %s%7.0f%s (30)",
        __ColorValue__(simSteps, 25, 15),
        simSteps,
        GUI.MarkupColor.White) ..
    GUI.MarkupNewline ..
    string.format("  Simulation Time:          %7.2f ms", avgSimulateTime);

    profileInfoPanel:setMarkupText(GUI.MarkupColor.White .. profileInfo)
end


function GUI_CreateCameraAndProfileInfo()
    for index, color in pairs(GUI.MarkupColorTable) do
        Sandbox:SetMarkupColor(index, color)
    end
	
	cameraInfoPanel = GUI_CreateCameraInfo()
	profileInfoPanel = GUI_CreateProfileInfo()
end


function GUI_HandleKeyEvent(keycode, pressed)
	if not pressed then return end

	if (keycode == OIS.KC_F5) then
        local isShow = profileInfoPanel:isVisible()
        profileInfoPanel:setVisible(not isShow)
    elseif (keycode == OIS.KC_F6) then
        local isShow = cameraInfoPanel:isVisible()
        cameraInfoPanel:setVisible(not isShow)
    elseif (keycode == OIS.KC_F7) then
        --Sandbox:SetDrawPhysicsWorld(drawDebug)
    end
end


function GUI_WindowResized(width, height)
	local dimension1 = cameraInfoPanel:GetDimension()
    local ui_posx1 = width - dimension1.x - 20;
    local ui_posy1 = 35 + dimension1.y;
    cameraInfoPanel:setPosition(Vector2(ui_posx1, ui_posy1))

    local dimension2 = profileInfoPanel:GetDimension()
    local ui_posx2 = width - dimension2.x - 20;
    local ui_posy2 = 20;
    profileInfoPanel:setPosition(Vector2(ui_posx2, ui_posy2))
end


function GUI_CreateSandboxText(infoText, boxSize)
    local screenWidth = GameManager:getScreenWidth()
    local screenHeight = GameManager:getScreenHeight()
    
    --local ui_width, ui_height = 300, 180;
    local ui_width, ui_height = boxSize.w, boxSize.h;
    local sandboxMenuInfo = Sandbox:CreateUIComponent()
    local ui_posx = screenWidth - ui_width - 20;
    local ui_posy = screenHeight - ui_height - 35;
    sandboxMenuInfo:setPosition(Vector2(ui_posx, ui_posy))
    sandboxMenuInfo:setDimension(Vector2(ui_width, ui_height))
    sandboxMenuInfo:setTextMargin(10, 10);

    local startColor = GUI.Palette.DarkBlueGradient
    local endenColor = GUI.Palette.DarkBlackGradient
    sandboxMenuInfo:setGradientColor(Gorilla.Gradient_NorthSouth, startColor, endenColor)

    local markupText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Space: shoot block" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F5: toggle performance information" .. GUI.MarkupNewline ..
        "F6: toggle camera information" .. GUI.MarkupNewline ..
        "F7: toggle physics debug"
    sandboxMenuInfo:setMarkupText((infoText and infoText or markupText))

    return sandboxMenuInfo
end

function GUI_CreateSandbox3Text()
    local ui_width, ui_height = 300, 180;
    local screenWidth = GameManager:getScreenWidth()
    local screenHeight = GameManager:getScreenHeight()

    local uiComponent = Sandbox:CreateUIComponent()
    local ui_posx = screenWidth - ui_width - 20;
    local ui_posy = screenHeight - ui_height - 35;
    uiComponent:setPosition(Vector2(ui_posx, ui_posy))
    uiComponent:setDimension(Vector2(ui_width, ui_height))
    uiComponent:setTextMargin(10, 10);

    local startColor = GUI.Palette.DarkBlueGradient
    local endenColor = GUI.Palette.DarkBlackGradient
    uiComponent:setGradientColor(Gorilla.Gradient_NorthSouth, startColor, endenColor)

    
    uiComponent:setMarkupText(markupText)

    return uiComponent
end