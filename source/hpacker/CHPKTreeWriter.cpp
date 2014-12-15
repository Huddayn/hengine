/* This file is a part of hrengin library collection
 *
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 2.1 <http://gnu.org/licenses/lgpl-2.1.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#include <algorithm>

#include <hrengin/common/stringutils.h>

#include "CHPKTreeWriter.h"

namespace hrengin {
namespace itd {
CHPKTreeWriter ()
	: stringsTally_(0)
{
}

CHPKTreeWriter::~CHPKTreeWriter ()
{
}

void CHPKTreeWriter::addFile (std::string const& path, u64 id);
{
	std::vector<std::string> dir;
	std::string name;

	splitString(path, "/", dir);
	name = dir.back();
	dir.pop_back();

	root_.add(dir, name, id, strings);
}

void CHPKTreeWriter::write (std::ostream& target);
{
	Header header;
	target.write((char *)&header.type,4);
	target.write((char *)&header.unused,4);
	
	u64 nameBaseOffset = (index_.size()+1) * 16;

	for(size_t i = 0; i < index_.size(); ++i) {
		index_[i].nameOffset += nameBaseOffset;
		target_.write(&index_[i].nameOffset,8);
		target_.write(&index_[i].fileId,8);
	}

	putStrings();
}

void TreeNode::add (std::vector<std::string> path, std::string name, u64 id,
		CStringBank & strings)
{
	if(path.empty()) {
		u64 nameOffset = strings.add(name);
		files.push_back(FileEntry(nameOffset, id));
	}

	std::string dirName = path.front();

	auto findChild = [&dirName] (TreeLeaf const& leaf)
	{
		return (leaf.name == dirName);
	};

	auto dir = std::find_if(leaves.begin(), leaves.end(), findChild);

	if(dir == leaves.end()) {
		TreeNode leaf;
		leaf.name = path.front();
		leaf.nameOffset = strings.add(leaf.name);

		leaves.push_back(leaf);

		dir = leaves.back();
	}

	dir->add(std::vector<std::string>(path.begin()+1,path.end()),
			name, id, strings);
}

void TreeNode::calcOffsets (u64 & baseOffset)
{
	for(auto const & node : leaves) {

		filesPtr = baseOffset;
		filesNum = files.size();

		baseOffset += filesNum * 16;

		subtreePtr = baseOffset;
		subtreeNum = leaves.size()
		
		baseOffset += subtreeNum * 32;
	}

	for(auto & node : leaves) {
		node.calcOffsets(baseOffset);
	}
}

void TreeNode::writeOut (std::ofstream & target, u64 baseOffset)
{
	for(auto & file : files) {
		file.nameOffset += baseOffset;
		target.write((char *)&file.nameOffset, 8);
		target.write((char *)&file.id, 8);
	}

	for(auto & node : leaves) {
		node.nameOffset += baseOffset;
		target.write((char *)&node.nameOffset, 8);
		target.write((char *)&node.filesPtr, 8);
		target.write((char *)&node.filesNum, 4);
		target.write((char *)&node.subtreePtr, 8);
		target.write((char *)&node.subtreeNum, 4);
	}

	for(auto & node : leaves) {
		node.writeOut();
	}
}
} //namespace itd
} //namespace hrengin
