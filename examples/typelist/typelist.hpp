#pragma once

template <typename T>
struct IdentityT
{
  using Type = T;
};

template <typename... Args>
struct TypeList
{ };