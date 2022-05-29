#include "behaviortree_cpp_v3/blackboard.h"

namespace BT{

void Blackboard::setPortInfo(std::string key, const PortInfo& info)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if( auto parent = parent_bb_.lock())
    {
        auto remapping_it = internal_to_external_.find(key);
        if( remapping_it != internal_to_external_.end())
        {
            parent->setPortInfo( remapping_it->second, info );
        }
    }

    auto it = storage_.find(key);
    if( it == storage_.end() )
    {
        storage_.emplace( key, Entry(info) );
    }
    else{
        auto old_type = it->second.port_info.type();
        if( old_type && old_type != info.type() )
        {
            throw LogicError( "Blackboard::set() failed: once declared, the type of a port shall not change. "
                             "Declared type [",     BT::demangle( old_type ),
                             "] != current type [", BT::demangle( info.type() ), "]" );
        }
    }
}

const PortInfo* Blackboard::portInfo(const std::string &key)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = storage_.find(key);
    if( it == storage_.end() )
    {
        return nullptr;
    }
    return &(it->second.port_info);
}

void Blackboard::addSubtreeRemapping(StringView internal, StringView external)
{
    internal_to_external_.insert( {static_cast<std::string>(internal), static_cast<std::string>(external)} );
}

void Blackboard::debugMessage() const
{
    for(const auto& entry_it: storage_)
    {
        auto port_type = entry_it.second.port_info.type();
        if( !port_type )
        {
            port_type = &( entry_it.second.value.type() );
        }

        std::cout <<  entry_it.first << " (" << demangle( port_type ) << ") -> ";

        if( auto parent = parent_bb_.lock())
        {
            auto remapping_it = internal_to_external_.find( entry_it.first );
            if( remapping_it != internal_to_external_.end())
            {
                std::cout << "remapped to parent [" << remapping_it->second << "]" <<std::endl;
                continue;
            }
        }
        std::cout << ((entry_it.second.value.empty()) ? "empty" : "full") <<  std::endl;
    }
}

std::vector<StringView> Blackboard::getKeys() const
{
    if( storage_.empty() ){
        return {};    
    }
    std::vector<StringView> out;
    out.reserve( storage_.size() );
    for(const auto& entry_it: storage_)
    {
        out.push_back( entry_it.first );
    }
    return out;
}

std::string Blackboard::saveToString() const
{
  std::unique_lock<std::mutex> lock1(mutex_);
  std::string out;
  for(const auto& it: storage_)
  {
    std::unique_lock<std::mutex> lock2(entry_mutex_);
    const Entry& entry = it.second;
    if( entry.value.empty() )
    {
        continue;
    }
    if( entry.value.isString() )
    {
      out += it.first + ": " + entry.value.cast<std::string>() + "\n";
    }
    else if( entry.value.type() == typeid(int64_t) )
    {
      out += it.first + ": " + std::to_string(entry.value.cast<int64_t>()) + "\n";
    }
    else if( entry.value.type() == typeid(uint64_t) )
    {
      out += it.first + ": " + std::to_string(entry.value.cast<uint64_t>()) + "\n";
    }
    else if( entry.value.type() == typeid(double) )
    {
      out += it.first + ": " + std::to_string(entry.value.cast<double>()) + "\n";
    }
  }
  return out;
}

void Blackboard::loadFromString(const std::string &str)
{
  std::istringstream ss(str);
  std::string line;
  while (std::getline(ss, line))
  {
    std::unique_lock<std::mutex> lock2(entry_mutex_);

    size_t pos = line.find(": ");
    std::string key = line.substr(0, pos);
    auto it = storage_.find(key);
    if( it == storage_.end() )
    {
      continue;
    }
    std::string value = line.substr(pos + 2);

    Entry& entry = it->second;
    if( entry.port_info.type() == &typeid(std::string) )
    {
      entry.value = Any(value);
    }
    else if( entry.port_info.type() == &typeid(int64_t) )
    {
      entry.value = Any( convertFromString<int64_t>(value) );
    }
    else if( entry.port_info.type() == &typeid(uint64_t) )
    {
      entry.value = Any( convertFromString<uint64_t>(value) );
    }
    else if( entry.port_info.type() == &typeid(float) )
    {
      entry.value = Any( convertFromString<float>(value) );
    }
    else if( entry.port_info.type() == &typeid(int) )
    {
      entry.value = Any( convertFromString<int>(value) );
    }
    else if( entry.port_info.type() == &typeid(unsigned) )
    {
      entry.value = Any( convertFromString<unsigned>(value) );
    }
    else if( entry.port_info.type() == &typeid(bool) )
    {
      entry.value = Any( convertFromString<bool>(value) );
    }
    else if( entry.port_info.type() == &typeid(NodeStatus) )
    {
      entry.value = Any( convertFromString<NodeStatus>(value) );
    }
  }
}

}
