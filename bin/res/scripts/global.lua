function Vector3(...)
	return Ogre.Vector3(unpack{...})
end

function Quaternion(xr, yr, zr)
	return QuaternionFromRotationDegrees(xr, yr, zr)
end