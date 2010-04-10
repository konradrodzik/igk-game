#pragma once
#include "Common.h"

enum EContactType {
	CT_ADD,
	CT_KEEP,
	CT_REMOVE,
	CT_RESULTS
};

class Body;
typedef FastDelegate7<Body*, Body*, EContactType, float, float, float, void*> CollisionDelegate;
class CWorld;

class Body {
protected:
	b2Body* m_body;
	CollisionDelegate m_onCollision;

public:
	std::string label;
	bool shouldDie;

	void applyForce(float forceX,float forceY, float pointX, float pointY ) {
		m_body->ApplyForce(b2Vec2(forceX, forceY), b2Vec2(pointX, pointY));
	}

	void applyImpulse(float forceX,float forceY) {
		m_body->ApplyImpulse(b2Vec2(forceX, forceY), m_body->GetWorldCenter());
	}

	void applyForce(float forceX,float forceY ) {
		m_body->ApplyForce(b2Vec2(forceX, forceY), m_body->GetWorldCenter());
	}

	void applyTorque(float torque ) {
		m_body->ApplyTorque(torque);
	}

	b2Body* getBody() {
		return m_body;
	}

	//! set on contact function
	void setOnContact(CollisionDelegate deleg) {
		m_onCollision = deleg;
	}

	//! on collision/contact
	virtual void onContact(Body* otherBody, EContactType type, float friction, float relVelx, float relVely, void* deleteMe) {
		if(!m_onCollision.empty())
		{
			m_onCollision(this, otherBody, type, friction, relVelx, relVely, deleteMe);
		}
	}

	virtual void draw() {}

	Body()
	{
		shouldDie = false;
	}

	virtual ~Body();
};


struct contactInfo {
	Body* target, *other;
	float friction;
	float velx, vely;
	EContactType type;
	void* deleteMe;

	contactInfo(Body* targ, Body* other, EContactType type, float fric, float velx, float vely, void* deleteme)
		: target(targ)
		, other(other)
		, friction(fric)
		, velx(velx)
		, vely(vely)
		, type(type)
		, deleteMe(deleteme){};
};

extern std::vector<contactInfo>* contacts;

class ContactListener : public b2ContactListener{
public:
	void Add(const b2ContactPoint* point)
	{
		// handle add point
		Body* a = (Body*)point->shape1->GetBody()->GetUserData();
		Body* b = (Body*)point->shape2->GetBody()->GetUserData();
		// 		if(a)
		// 			a->onContact(b, CT_ADD, point->friction, point->velocity.x, point->velocity.y);
		// 
		// 		if(b)
		// 			b->onContact(a, CT_ADD, point->friction, point->velocity.x, point->velocity.y);

		void* deleteMe = 0;
		if(!a) 
			deleteMe = a;

		if(!b) 
			deleteMe = b;

		contacts->push_back( contactInfo(a, b, CT_ADD, point->friction, point->position.x, point->position.y, deleteMe));
		contacts->push_back( contactInfo(b, a, CT_ADD, point->friction, point->position.x, point->position.y, deleteMe));
	}

	void Persist(const b2ContactPoint* point)
	{
		// handle persist point
		Body* a = (Body*)point->shape1->GetBody()->GetUserData();
		Body* b = (Body*)point->shape2->GetBody()->GetUserData();
		// 		if(a)
		// 			a->onContact(b, CT_KEEP, point->friction, point->velocity.x, point->velocity.y);
		// 		
		// 		if(b)
		// 			b->onContact(a, CT_KEEP, point->friction, point->velocity.x, point->velocity.y);
		void* deleteMe = 0;
		if(!a) 
			deleteMe = a;

		if(!b) 
			deleteMe = b;

		contacts->push_back( contactInfo(a, b, CT_KEEP, point->friction, point->position.x, point->position.y, deleteMe));
		contacts->push_back( contactInfo(b, a, CT_KEEP, point->friction, point->position.x, point->position.y, deleteMe));
	}

	void Remove(const b2ContactPoint* point)
	{
		// handle remove point
		Body* a = (Body*)point->shape1->GetBody()->GetUserData();
		Body* b = (Body*)point->shape2->GetBody()->GetUserData();
		// 		if(a)
		// 			a->onContact(b, CT_REMOVE, point->friction, point->velocity.x, point->velocity.y);
		// 
		// 		if(b)
		// 			b->onContact(a, CT_REMOVE, point->friction, point->velocity.x, point->velocity.y);
		void* deleteMe = 0;
		if(!a) 
			deleteMe = a;

		if(!b) 
			deleteMe = b;

		contacts->push_back( contactInfo(a, b, CT_REMOVE, point->friction, point->position.x, point->position.y, deleteMe));
		contacts->push_back( contactInfo(b, a, CT_REMOVE, point->friction, point->position.x, point->position.y,deleteMe));
	}

	void Result(const b2ContactResult* point)
	{
		// handle results
		Body* a = (Body*)point->shape1->GetBody()->GetUserData();
		Body* b = (Body*)point->shape2->GetBody()->GetUserData();
		// 		if(a)
		// 			a->onContact(b, CT_RESULTS, point->normalImpulse, point->tangentImpulse, 0);
		// 
		// 		if(b)
		// 			b->onContact(a, CT_RESULTS, point->normalImpulse, point->tangentImpulse, 0);
		void* deleteMe = 0;
		if(!a) 
			deleteMe = a;

		if(!b) 
			deleteMe = b;
		contacts->push_back( contactInfo(a, b, CT_RESULTS, point->normalImpulse, point->tangentImpulse, 0, deleteMe));
		contacts->push_back( contactInfo(b, a, CT_RESULTS, point->normalImpulse, point->tangentImpulse, 0, deleteMe));
	}
};

class World : public Singleton<World> {
protected:

	b2World* m_world;
	float m_gravity;
	ContactListener* m_ct;
	std::vector<contactInfo> m_contacts;

public:
	std::vector<b2Body*> m_shapes;
	World() {
		b2AABB worldBB;
		worldBB.lowerBound = b2Vec2(-800, -600);
		worldBB.upperBound = b2Vec2(800, 600);
		m_world = new b2World(worldBB, b2Vec2(0, -9.8), false);

		m_gravity = -9.8f;
		m_ct = new ContactListener();
		m_world->SetContactListener(m_ct);
		contacts = &m_contacts;
		
	} 

	void remove_last() {
		if(m_shapes.size() > 0)
		{	m_world->DestroyBody(m_shapes[m_shapes.size()-1]);
		m_shapes.pop_back();
		};
	}
	void refresh() {
// 		b2Body* cur = m_world->GetBodyList();
// 		b2Body* next;
// 		while(cur) {
// 			next = cur->GetNext();		
// 			m_world->DestroyBody(cur);
// 			cur = next;
// 		}

		for(int i=0; i < m_shapes.size(); ++i) {
			m_world->DestroyBody(m_shapes[i]);
		}
		m_shapes.clear();
	}

	float getGravitySign() {
		return m_gravity / fabs(m_gravity);
	}

	void swapGravity() {
		m_gravity*=-1;
		m_world->SetGravity(b2Vec2(0, m_gravity));
		b2Body* cur = m_world->GetBodyList();

		while(cur) {
			cur->WakeUp();
			cur =	cur->GetNext();
		}
	}

	b2World* getWorld() {
		return m_world;
	}

	~World() {
		delete m_world;
	}

	bool addShape(std::vector<D3DXVECTOR2>& vertices) {
		try {
			b2BodyDef ground;
			ground.position.Set(0,0);
			ground.userData = NULL;
			b2Body* groundBody = m_world->CreateBody(&ground);
			
			b2PolygonDef worldStatic;
			worldStatic.friction = 0.5f;
			worldStatic.restitution = 0.5f;

			for(int i=0; i < vertices.size(); ++i) {
				worldStatic.vertices[i].Set(vertices[i].x, vertices[i].y);
			}

			worldStatic.vertexCount = vertices.size();
			groundBody->CreateShape(&worldStatic);
			m_shapes.push_back(groundBody);

		} catch(...) {
			return false;
		}

		return true;
	}

	void update(float frameTime) {
		float32 timeStep = frameTime;
		//float32 timeStep = 1/30.0f;
		int32 iterations = 10;
		m_world->Step(timeStep, iterations);

		for(int i=0; i < m_contacts.size(); ++i)
		{
			if(m_contacts[i].target)
			{
				bool deleteMe = false;
				m_contacts[i].target->onContact(m_contacts[i].other, m_contacts[i].type, m_contacts[i].friction, m_contacts[i].velx, m_contacts[i].vely, m_contacts[i].deleteMe);
			}
		}

		m_contacts.clear();
	}
};

inline World* g_World() { 
	return World::getSingletonPtr(); 
}

class Sphere : public Body {
protected:
	Texture ball;
	float m_radius;
public:

	Sphere(float x, float y, float r, float density, float friction = 0.5f, float restitution = 0.5f, const char* texName = "ball.png")
		: ball(texName) {
			b2BodyDef def;
			def.position.Set(x,y);
			def.userData = this;
			m_body = g_World()->getWorld()->CreateBody(&def);

			b2CircleDef circle;
			circle.radius = r;
			circle.density = density;
			m_radius = r;
			circle.friction = friction;
			circle.restitution = restitution;
			m_body->CreateShape(&circle);
			if(density > 0.0f) 
				m_body->SetMassFromShapes();
	}

	void draw() {
		g_Renderer()->setRotation(m_body->GetPosition().x /*+ m_radius * 0.5f*/, m_body->GetPosition().y /*+ m_radius * 0.5f*/, m_body->GetAngle());
		ball.set();
		g_Renderer()->drawRect(m_body->GetPosition().x - m_radius, m_body->GetPosition().y - m_radius, m_radius*2, m_radius*2);
		g_Renderer()->setIdentity();
	}

	float getRadius() {
		return m_radius;
	}

};

class Joint {
protected:
	b2Joint* m_joint;
	float m_breakAt;
public:
	Joint()
		: m_joint(NULL) 
		, m_breakAt((float)INFINITE){

	}

	~Joint() {
		if(m_joint) g_World()->getWorld()->DestroyJoint(m_joint);
		m_joint = NULL;
	}

	void makeDistanceJoint(Body* a, Body* b, b2Vec2 pA, b2Vec2 pB, bool collisionOn = true) {
		b2DistanceJointDef def;
		def.Initialize(a->getBody(), b->getBody(), pA, pB);
		def.collideConnected = collisionOn;
		def.userData = this;

		m_joint = g_World()->getWorld()->CreateJoint(&def);
	}

	void makeDistanceJoint2(b2Body* a, b2Body* b, b2Vec2 pA, b2Vec2 pB, bool collisionOn = true) {
		b2DistanceJointDef def;
		def.Initialize(a, b, pA, pB);
		def.collideConnected = collisionOn;
		def.userData = this;

		m_joint = g_World()->getWorld()->CreateJoint(&def);
	}

	void makeAngleJoint(Body* a, Body* b, b2Vec2 point, bool collisionOn = true, bool motorOn = false, float maxAngle = 0.25f*b2_pi, float minAngle = -0.5f*b2_pi, bool limit = false, float motorSpeed = 0.0f, float maxMotorTorque = 10.0f) {
		b2RevoluteJointDef def;
		def.Initialize(a->getBody(), b->getBody(), point);
		def.collideConnected = collisionOn;
		def.lowerAngle =  minAngle; // -90 degrees
		def.upperAngle = maxAngle; // 45 degrees
		def.enableLimit = limit;
		def.maxMotorTorque = maxMotorTorque;
		def.motorSpeed = motorSpeed;
		def.enableMotor = motorOn;
		def.userData = this;
		m_joint = g_World()->getWorld()->CreateJoint(&def);
	}

	void makePulleyJoint(Body* a, Body* b, b2Vec2 pA, b2Vec2 pB, bool collisionOn = true, float length1 = 10.0f, float length2 = 20.0f, float ratio = 1.0f) {
		// 		b2PulleyJointDef jointDef;
		// 		jointDef.Initialize(a->getBody(), b->getBody(), pA, pB, pA, pB, ratio);
		// 		jointDef.maxLength1 = length1;
		// 		jointDef.maxLength2 = length2;
		// 		jointDef.userData = this;
		// 		m_joint = g_World()->getWorld()->CreateJoint(&jointDef);
	}

	void setBreakPoint(float powerLength) {
		m_breakAt = powerLength;
	}

	void draw() {
		if(!m_joint) return;
		/*getDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);*/
		getDevice()->SetTexture(0,0);
		g_Renderer()->drawLine(m_joint->GetAnchor1().x, m_joint->GetAnchor1().y, m_joint->GetAnchor2().x, m_joint->GetAnchor2().y, 0.1f);

		if(m_joint->GetReactionForce().Length() > m_breakAt) {
			g_World()->getWorld()->DestroyJoint(m_joint);
			m_joint = NULL;
		}
	}

};

class Box : public Body
{
	Texture boxTex;
	float sizex_, sizey_;

public:
	Box(float cx, float cy, float sizex, float sizey, float density, 
		float friction = 0.5f, const char* texName = "box.png")
		: boxTex(texName), sizex_(sizex), sizey_(sizey)
	{
		b2BodyDef def;
		def.position.Set(cx, cy);
		def.userData = this;
		m_body = g_World()->getWorld()->CreateBody(&def);
		if(!m_body) return;

		b2PolygonDef box;

		box.SetAsBox(sizex/2, sizey/2, b2Vec2(0, 0), 0);

		box.density = density;
		box.friction = friction;
		box.restitution = 0.5f;


		m_body->CreateShape(&box);
		m_body->SetMassFromShapes();
	}

	void draw() 
	{
		g_Renderer()->setRotation(m_body->GetPosition().x, m_body->GetPosition().y, m_body->GetAngle());
		boxTex.set();
		g_Renderer()->drawRect(m_body->GetPosition().x - sizex_/2, m_body->GetPosition().y - sizey_/2, 
			sizex_, sizey_);
		g_Renderer()->setIdentity();
	}
};

class PowerUp : public Sphere
{
public:
	float frictionCoeff;
	float jumpCoeff;

	PowerUp(float x, float y, float r, float density, float friction = 0.5f, float restitution = 0.5f, const char* texName = "ball3.png")
		: Sphere(x,y,r,density,friction,restitution, texName)
	{
		label = "$PowerUp";
		frictionCoeff = 1.0f;
		jumpCoeff = 1.0f;
	}

	static PowerUp * Cast(Body * body)
	{
		if(body->label ==  "$PowerUp")
		{
			return static_cast<PowerUp *>(body);
		}
		return NULL;
	}
};

inline Body::~Body() {
	if(m_body) {
		g_World()->getWorld()->DestroyBody(m_body);
		m_body = NULL;
	}
}