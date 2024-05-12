initState = 0
playState = 1
overState = 2
GameState = initState
size = 6
raws = 20
cols = 12
x = 5
y = 1
r = 0
loopCnt = 0
dy = 0
wall = 7
back = 8
clist = {8, 9, 10, 11, 12, 13, 14, 6, 7}
stage = {
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7},
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7},
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7},
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7}, 
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7}
}
ptnNo = 0
dx = 0
px = {0, 0, 0, 0}
py = {0, 0, 0, 0}
ptnxy = {
    {{-1, 0}, {1, 0}, {2, 0}},
    {{-1, -1}, {-1, 0}, {1, 0}},
    {{-1, 0}, {1, -1}, {1, 0}},
    {{-1, 0}, {0, 1}, {1, 1}},
    {{1, 0}, {0, 1}, {-1, 1}},
    {{-1, 0}, {0, -1}, {1, 0}},
    {{1, 0}, {0, 1}, {1, 1}}
}

ff = 0

function setPosition()
  px[1], py[1] = x, y
  r = r % 4
  _r = r
  for i = 1, 3 do
      _x, _y = ptnxy[ptnNo][i][1], ptnxy[ptnNo][i][2]
      for j = 1, _r do
          _w = _x
          _x = _y
          _y = -_w
      end
      px[i + 1] = x + _x
      py[i + 1] = y + _y
  end
end

function setPtn()
  setPosition()
  for i = 1, 4 do
      stage[py[i]][px[i]] = ptnNo
  end
end

function delPtn()
  setPosition()
  for i = 1, 4 do
      stage[py[i]][px[i]] = back
  end
end

function collision()
  setPosition()
  for i = 1, 4 do
      if stage[py[i]][px[i]] ~= back then
          return true
      end
  end
  return false
end

function complete()
  for _y = 1, raws - 1 do
      _flag2 = 1
      for _x = 2, cols - 2 do
          if stage[_y][_x] == back then
              _flag2 = 0
          end
          if _flag2 == 1 then
              for upy = _y - 1, 1, -1 do
                  for _x = 2, cols - 2 do
                      stage[upy + 1][_x] = stage[upy][_x]
                  end
              end
          end
      end
  end
end

function drawStage()
  for by = 1, raws do
      for bx = 1, cols do
        no = stage[by][bx]
        -- color(clist[no])
        -- spr8(1,size * bx, size * by)
        fillrect(size * bx, size * by, size, size, clist[no])
      end
  end
  
end

function play()
  -- delPtn()
  -- dy, dx, dr = 0, 0, 0
  -- while loopCnt == 5 do
  --     dy = 1
  --     loopCnt = 0
  -- end
  -- loopCnt = loopCnt + 1
  -- if btn(2) then
  --     dx = 1
  --     -- sfx(6)
  -- end
  -- if btn(1) then
  --     dx = -1
  --     -- sfx(6)
  -- end
  -- if btn(4) then
  --     dr = 1
  --     -- sfx(12)
  -- end
  -- if btn(3) then
  --     ff = 1
  --     -- sfx(0)
  -- end
  -- if ff then
  --     dy = 1
  -- end
  -- y = y + dy
  -- x = x + dx
  -- r = r + dr

  -- if collision() == true then
  --     y = y - dy
  --     x = x - dx
  --     r = r - dr
  --     ff = 0
  --     if dy == 1 and dx == 0 and dr == 0 then
  --         setPtn()
  --         complete()
  --         y = 1
  --         x = 5
  --         r = 0
  --         ptnNo = flr(rnd(7))
  --         if collision() == true then
  --             GameState = overState
  --         end
  --     end
  -- end
  -- setPtn()
  drawStage()
end


function setup()
  fillrect(20,20,8,8,7)
end

function _init()--1回だけ
    setup()
end

function input()

  -- if btn(1) >= 1 then x = x-3 end
  -- if btn(2) >= 1 then x = x+3 end
  -- if btn(3) >= 1 then 
  --   y = y-3
  --   if y<0 then y = 128 end
  -- end
  -- if btn(4) >= 1 then y = y+3 end

  -- if btn(3) >= 1 then 
  --   cn = cn + 1
  --  end
  
end

function _update()--ループします
  input()
end

function _draw()--ループします

  -- fillrect(tp(0),tp(1),8,8,10)
  play()
  -- if GameState == initState then

  -- elseif GameState == playState then
  --     play()
  -- elseif GameState == overState then
  --     over()
  -- end

end


-- function _init()

-- end

-- function input()
--   -- if btn(1) then
--   --   for by = 1, raws do
--   --       stage[by][1] = wall
--   --       stage[by][cols] = wall
--   --       for bx = 2, cols - 1 do
--   --           stage[by][bx] = back
--   --           if by == raws then
--   --               stage[by][bx] = wall
--   --           end
--   --       end
--   --   end
--   --   x, y, r = 5, 1, 0
--   --   ptnNo = flr(rnd(7))
--   --   GameState = playState
--   -- end
-- end

-- function _update()--ループします
--   input()
-- end

-- function _draw()
--   play()
--   -- if GameState == initState then

--   -- elseif GameState == playState then
--   --     play()
--   -- elseif GameState == overState then
--   --     over()
--   -- end
-- end