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

import 'package:flutter/material.dart';
import 'emergency_demo_page.dart';

class SplashPage extends StatefulWidget {
  const SplashPage({super.key});
  @override
  State<SplashPage> createState() => _SplashPageState();
}

class _SplashPageState extends State<SplashPage>
    with SingleTickerProviderStateMixin {
  // Parameter
  static const Duration _kFadeDuration =
      Duration(milliseconds: 800); // Fade time
  static const Duration _kSplashHold = Duration(seconds: 3); // Display time
  static const String _kLogoAsset =
      'assets/splash_png/yoriito_api_logo_small-1536.png';
  static const double _kLogoWidth = 384;

  // Fade-in animation
  late final AnimationController _ctrl = AnimationController(
    vsync: this,
    duration: _kFadeDuration,
  )..forward();

  @override
  void initState() {
    super.initState();
    // Transition to demo page after specified time. (splash → main screen)
    Future.delayed(_kSplashHold, () {
      if (!mounted) return;
      Navigator.of(context).pushReplacement(
        MaterialPageRoute(builder: (_) => const EmergencyDemoPage()),
      );
    });
  }

  @override
  void dispose() {
    _ctrl.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: FadeTransition(
        opacity: _ctrl.drive(CurveTween(curve: Curves.easeOut)),
        child: Center(
          child: Image.asset(
            _kLogoAsset,
            width: _kLogoWidth,
            fit: BoxFit.contain,
          ),
        ),
      ),
    );
  }
}
