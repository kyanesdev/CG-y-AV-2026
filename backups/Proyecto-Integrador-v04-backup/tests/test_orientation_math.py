#!/usr/bin/env python3
"""Numerical smoke test for quaternion/Euler convention.

Independent of GLM/FDM and therefore NOT a substitute for running AeroIUA.
"""
import math


def mul(a, b):
    w,x,y,z=a; v,i,j,k=b
    return (w*v-x*i-y*j-z*k, w*i+x*v+y*k-z*j,
            w*j-x*k+y*v+z*i, w*k+x*j-y*i+z*v)


def angle_axis(angle,axis):
    c=math.cos(angle/2); s=math.sin(angle/2)
    return (c,axis[0]*s,axis[1]*s,axis[2]*s)


def rotate(q,vec):
    c=(q[0],-q[1],-q[2],-q[3])
    p=mul(mul(q,(0,)+tuple(vec)),c)
    return p[1:]


def euler_canonical(q):
    c0=rotate(q,(1,0,0)); c1=rotate(q,(0,1,0)); c2=rotate(q,(0,0,1))
    theta=math.asin(max(-1,min(1,-c0[2])))
    phi=math.atan2(c1[2],c2[2]); psi=math.atan2(c0[1],c0[0])
    return phi,theta,psi


def orientation(phi,theta,psi):
    return mul(mul(angle_axis(psi,(0,0,1)),angle_axis(theta,(0,1,0))),angle_axis(phi,(1,0,0)))


def diff(a,b):
    return max(abs(x-y) for x,y in zip(a,b))

# A full pitch rotation must continue through +90, invert, then return upright.
last=None
for i in range(361):
    angle=math.radians(i)
    original=angle_axis(angle,(0,1,0))
    phi,theta,psi=euler_canonical(original)
    reconstructed=orientation(phi,theta,psi)
    forward=rotate(original,(1,0,0))
    roundtrip=rotate(reconstructed,(1,0,0))
    assert diff(forward,roundtrip)<1e-5, (i,forward,roundtrip)
    if last is not None:
        assert diff(forward,last)<0.018, i
    last=forward
assert rotate(angle_axis(math.pi,(0,1,0)),(1,0,0))[0]<-0.999
print('OK: 360-degree quaternion pitch and continuous orientation')
