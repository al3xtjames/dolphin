// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DiscIO/VolumeFileBlobReader.h"

#include "DiscIO/Filesystem.h"
#include "DiscIO/Volume.h"

namespace DiscIO
{
std::unique_ptr<VolumeFileBlobReader> VolumeFileBlobReader::Create(const Volume& volume,
                                                                   const Partition& partition,
                                                                   const std::string& file_path)
{
  const FileSystem* file_system = volume.GetFileSystem(partition);
  if (!file_system)
    return nullptr;

  std::unique_ptr<FileInfo> file_info = file_system->FindFileInfo(file_path);
  if (!file_info || file_info->IsDirectory())
    return nullptr;

  return std::unique_ptr<VolumeFileBlobReader>{
      new VolumeFileBlobReader(volume, partition, std::move(file_info))};
}

VolumeFileBlobReader::VolumeFileBlobReader(const Volume& volume, const Partition& partition,
                                           std::unique_ptr<FileInfo> file_info)
    : m_volume(volume), m_partition(partition), m_file_info(std::move(file_info))
{
}

u64 VolumeFileBlobReader::GetDataSize() const
{
  return m_file_info->GetSize();
}

u64 VolumeFileBlobReader::GetRawSize() const
{
  return GetDataSize();
}

bool VolumeFileBlobReader::Read(u64 offset, u64 length, u8* out_ptr)
{
  if (offset + length > m_file_info->GetSize())
    return false;

  return m_volume.Read(m_file_info->GetOffset() + offset, length, out_ptr, m_partition);
}
}  // namespace DiscIO
