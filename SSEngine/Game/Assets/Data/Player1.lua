Pawn = 
{
	name = "Player1",
	type = "Player",
	
	mobility = "Movable",
	
	boundingBox = {
		center = {0.0, 0.0, 0.0},
		extend = {4.5, 4.5, 0.0}
	},
	
	renderObject = {
		path = "Assets\\Texture\\Player.dds",
		priority = 3000
	},
	
	physicsObject = {
		mass = 1.0,
		drag = 0.001
	},
	
	collisionObject = {
		collideType = "Block",
		active = true
	},
	
	HP = 2,
}