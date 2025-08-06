#!/bin/bash
cd build
echo "Running game and monitoring player-enemy collisions..."
echo "Move player into enemies to test collision damage"
echo "Press Ctrl+C to stop"
echo "================================"
./bin/2DEngine 2>&1 | grep -E "(Enemy-Player collision|contact damage|HealthSystem.*Applied.*damage|player.*health|CollisionSystem.*Rule)"