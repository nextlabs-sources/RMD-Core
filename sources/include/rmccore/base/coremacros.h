//
//  coremacros.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_BASE_COREMACROS_H__
#define __RMCCORE_BASE_COREMACROS_H__

#include <cstdint>

#ifndef Add2Ptr
#define Add2Ptr(P, O)   ((void*)((uint_ptr)(P) + (uint32_t)(O)))
#endif

#endif /* __RMCCORE_BASE_COREMACROS_H__ */
