// SPDX-License-Identifier: Apache-2.0
//
// Copyright 2026 JASPAR
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../services/emergency_rpc.dart';

class EmergencyDemoPage extends StatefulWidget {
  const EmergencyDemoPage({super.key});
  @override
  State<EmergencyDemoPage> createState() => _EmergencyDemoPageState();
}

class _EmergencyDemoPageState extends State<EmergencyDemoPage> {
  // Toggle state
  bool _isOn = false;
  // Toggle availability
  bool _isAvailable = false;

  // UI image
  final ImageProvider _toggleOn =
      const AssetImage('assets/emergency_png/toggle_on.png');
  final ImageProvider _toggleOff =
      const AssetImage('assets/emergency_png/toggle_off.png');

  // Toggle size
  final Rect _toggleRect = const Rect.fromLTWH(96, 229, 220, 220);

  // Stream
  StreamSubscription? _pollSub;

  // Focus node
  final FocusNode _focusNode = FocusNode();

  @override
  void initState() {
    super.initState();
    // Executed after the context is available.
    WidgetsBinding.instance.addPostFrameCallback((_) {
      precacheImage(_toggleOn, context);
      precacheImage(_toggleOff, context);
      _focusNode.requestFocus();
      _startTelemetry(interval_ms: 100);
    });
  }

  @override
  void dispose() {
    _focusNode.dispose();
    _pollSub?.cancel();
    super.dispose();
  }

  // --- Telemetry start ---
  void _startTelemetry({int interval_ms = 100}) {
    _pollSub?.cancel();
    _pollSub =
        EmergencyRpc.pollToggleAvailability(interval_ms: interval_ms).listen(
      (ret) {
        if (mounted) {
          setState(() => _isAvailable = ret.isAvailable);
        }
      },
      onError: (e) {
        if (!mounted) return;
        ScaffoldMessenger.of(
          context,
        ).showSnackBar(
            SnackBar(content: Text('Availability polling error: $e')));
        setState(() => _isOn = false);
        _pollSub?.cancel();
        _pollSub = null;
      },
    );
  }

  // --- Toggle state setting ---
  Future<void> _handleToggle() async {
    try {
      if (_isAvailable) {
        await EmergencyRpc.setToggleState(!_isOn);
        setState(() => _isOn = !_isOn);
      }
    } catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(SnackBar(content: Text('Server not responding: $e')));
    }
  }

  // --- UI ---
  @override
  Widget build(BuildContext context) {
    return RawKeyboardListener(
        focusNode: _focusNode,
        onKey: (event) {
          if (event is RawKeyDownEvent &&
              !event.repeat &&
              event.logicalKey == LogicalKeyboardKey.space) {
            _handleToggle();
          }
        },
        child: Scaffold(
          // No AppBar
          body: Stack(
            children: [
              // Draws a UI image. (ON/OFF)
              Image(image: _isOn && _isAvailable ? _toggleOn : _toggleOff),

              // Draws the toggle state.
              Positioned(
                left: 0,
                right: 0,
                top: 400,
                child: Center(
                  child: Text(
                    _isAvailable ? (_isOn ? 'ON' : 'OFF') : 'UNAVAILABLE',
                    style: const TextStyle(
                        fontSize: 20,
                        color: Colors.white,
                        fontWeight: FontWeight.bold),
                  ),
                ),
              ),

              // Draws the operation instruction message.
              Positioned(
                  left: 0,
                  right: 0,
                  top: 500,
                  child: Center(
                      child: Text(
                    _isAvailable ? 'Press Space to toggle.' : '',
                    style: const TextStyle(
                        fontSize: 20,
                        color: Colors.white,
                        fontWeight: FontWeight.bold),
                  ))),

              // Detecting taps on the toggle area.
              GestureDetector(onTapDown: (details) {
                if (_toggleRect.contains(details.localPosition))
                  _handleToggle();
              }),
            ],
          ),
        ));
  }
}
