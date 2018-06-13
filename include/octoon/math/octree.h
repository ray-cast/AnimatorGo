#ifndef OCTOON_MATH_OCTREE_H_
#define OCTOON_MATH_OCTREE_H_

#include <memory> // unique_ptr
#include <iostream> // istream, ostream

namespace octoon
{
	namespace math
	{
		template<typename ValueType, typename IndexType = std::int32_t, typename SizeType = std::uint32_t>
		class Octree
		{
		public:
			using value_t = ValueType;
			using index_t = IndexType;
			using size_t = SizeType;

			enum class node_t : std::uint8_t
			{
				branch,
				leaf,
			};

			class OctreeNode
			{
			public:
				OctreeNode(node_t type) noexcept : _type(type) {}

				node_t type() const noexcept { return _type; }
				char const * type_name() const noexcept { return _type == node_t::leaf ? "left" : "branch"; }

				virtual void set(value_t&& t) noexcept(false) = 0;
				virtual void set(const value_t& t) noexcept(false) = 0;

				virtual value_t& get() noexcept(false) = 0;
				virtual const value_t& get() const noexcept(false) = 0;

				virtual std::unique_ptr<OctreeNode> clone() const = 0;

				OctreeNode& operator=(value_t&& t) { this->set(t); return *this; }
				OctreeNode& operator=(const value_t& t) { this->set(t); return *this; }

			public:
				friend bool operator==(const OctreeNode& node, const value_t& v)
				{
					return node.get() == v;
				}

				friend bool operator!=(const OctreeNode& node, const value_t& v)
				{
					return !(node == v);
				}

				friend std::ostream& operator<<(std::ostream& o, const OctreeNode& node) 
				{
					o << node.get(); 
					return o; 
				}

			private:
				node_t _type;
			};

			class OctreeLeaf final : public OctreeNode
			{
			public:
				OctreeLeaf() noexcept 
					: OctreeNode(node_t::leaf)
					, _value()
				{
				}

				virtual ~OctreeLeaf() noexcept 
				{
				};

				virtual void set(value_t&& t) noexcept override 
				{ 
					_value = std::move(t); 
				};

				virtual void set(const value_t& t) noexcept override 
				{
					_value = t; 
				};

				virtual value_t& get() noexcept override
				{
					return _value; 
				};

				virtual const value_t& get() const noexcept override
				{
					return _value; 
				};

				virtual std::unique_ptr<OctreeNode> clone() const noexcept override 
				{
					return std::make_unique<OctreeLeaf>(*this); 
				}

			private:
				value_t _value;
			};

			class OctreeBranch final : public OctreeNode
			{
			public:
				OctreeBranch() noexcept
					: OctreeNode(node_t::branch) 
				{
				}

				OctreeBranch(const OctreeBranch& copy) noexcept
					: OctreeNode(node_t::branch) 
				{
					for (std::uint8_t x = 0; x < 8; x++)
						_children[x] = copy._children[x]->clone();
				}

				virtual ~OctreeBranch() noexcept
				{
				}

				OctreeNode& at(size_t size, index_t x, index_t y, index_t z) noexcept
				{
					assert(x >= -((index_t)size / 2) && x <= (index_t)size / 2);
					assert(y >= -((index_t)size / 2) && y <= (index_t)size / 2);
					assert(z >= -((index_t)size / 2) && z <= (index_t)size / 2);

					OctreeNode* node = this;

					while (size != 1)
					{
						size = size >> 1;
						node = &dynamic_cast<OctreeBranch*>(node)->next(size, x, y, z);
					}
	
					return *node;
				}

				const OctreeNode& at(size_t size, index_t x, index_t y, index_t z) const noexcept
				{
					assert(x >= -((index_t)size / 2) && x <= (index_t)size / 2);
					assert(y >= -((index_t)size / 2) && y <= (index_t)size / 2);
					assert(z >= -((index_t)size / 2) && z <= (index_t)size / 2);

					OctreeNode* node = this;

					while (size != 1)
					{
						size = size >> 1;
						node = &dynamic_cast<OctreeBranch*>(node)->next(size, x, y, z);
					}

					return *node;
				}

				void erase(size_t size, index_t x, index_t y, index_t z) noexcept
				{
					assert(x >= -((index_t)size / 2) && x <= (index_t)size / 2);
					assert(y >= -((index_t)size / 2) && y <= (index_t)size / 2);
					assert(z >= -((index_t)size / 2) && z <= (index_t)size / 2);

					OctreeBranch* node = this;

					while (size != 1)
					{
						size = size >> 1;
						
						index_t xx = !!(x & size);
						index_t yy = !!(y & size);
						index_t zz = !!(z & size);

						auto index = xx + yy * 2 + zz * 4;
						if (!node->_children[index])
							return;

						if (size == 1)
							node->_children[index].reset();

						node = dynamic_cast<OctreeBranch*>(&*node->_children[index]);
					}
				}

				virtual void set(value_t&& t) noexcept(false) override
				{
					throw std::runtime_error("Invalid Call.");
				};

				virtual void set(const value_t& t) noexcept(false) override
				{
					throw std::runtime_error("Invalid Call.");
				};

				virtual value_t& get() noexcept(false) override
				{
					throw std::runtime_error("Invalid Call.");
				};

				virtual const value_t& get() const noexcept(false) override
				{
					throw std::runtime_error("Invalid Call.");
				};

				virtual std::unique_ptr<OctreeNode> clone() const noexcept
				{
					return std::make_unique<OctreeBranch>(*this);
				}

			private:
				OctreeNode& next(size_t size, index_t x, index_t y, index_t z) noexcept
				{
					index_t xx = !!(x & size);
					index_t yy = !!(y & size);
					index_t zz = !!(z & size);

					auto index = xx + yy * 2 + zz * 4;
					if (!_children[index])
					{
						if (size == 1)
							_children[index] = std::make_unique<OctreeLeaf>();
						else
							_children[index] = std::make_unique<OctreeBranch>();
					}

					return *_children[index];
				}

				const OctreeNode& next(size_t size, index_t xx, index_t yy, index_t zz) const noexcept
				{
					index_t xx = !!(x & size);
					index_t yy = !!(y & size);
					index_t zz = !!(z & size);

					auto index = xx + yy * 2 + zz * 4;

					assert(_children[index]);

					return *_children[index];
				}

			private:
				std::unique_ptr<OctreeNode> _children[8];
			};

		public:
			Octree() noexcept
				: _root(std::make_unique<OctreeBranch>())
				, _size(512)
			{
			}

			explicit Octree(size_t size) noexcept
				: _root(std::make_unique<OctreeBranch>())
				, _size(size)
			{
				assert(((size - 1 & size) & size) == 0);
			}

			virtual ~Octree() noexcept
			{
			}

			OctreeNode& at(index_t x, index_t y, index_t z) noexcept
			{
				assert(x >= -((index_t)_size / 2) && x <= (index_t)_size / 2);
				assert(y >= -((index_t)_size / 2) && y <= (index_t)_size / 2);
				assert(z >= -((index_t)_size / 2) && z <= (index_t)_size / 2);

				return _root->at(_size, x, y, z);
			}

			const OctreeNode& at(index_t x, index_t y, index_t z) const noexcept
			{
				assert(x >= -((index_t)_size / 2) && x <= (index_t)_size / 2);
				assert(y >= -((index_t)_size / 2) && y <= (index_t)_size / 2);
				assert(z >= -((index_t)_size / 2) && z <= (index_t)_size / 2);

				return _root->at(_size, x, y, z);
			}

			void erase(index_t x, index_t y, index_t z)
			{
				assert(x >= -((index_t)_size / 2) && x <= (index_t)_size / 2);
				assert(y >= -((index_t)_size / 2) && y <= (index_t)_size / 2);
				assert(z >= -((index_t)_size / 2) && z <= (index_t)_size / 2);

				_root->erase(_size, x, y, z);
			}

			std::size_t nodes() const
			{
				std::size_t cnt = 0;

				auto eval = [&](const OctreeNode*) -> bool
				{
					return (++cnt) ? true : false;
				};

				this->recursive(_root, eval);

				return cnt;
			}

			template<typename Function>
			void recursive(OctreeNode* node, Function call)
			{
				if (!node)
					return;

				if (node->type() == node_t::branch)
				{
					this->recursive(static_cast<OctreeBranch*>(node)->at(0), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(1), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(2), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(3), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(4), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(5), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(6), callback);
					this->recursive(static_cast<OctreeBranch*>(node)->at(7), callback);
				}
				else if (node->type() == Leaf)
				{
					callback(node);
				}
				else
				{
					assert(false);
				}
			}

			template<typename Function>
			void recursive(const OctreeNode* node, Function call) const
			{
				if (!node)
					return;

				if (node->type() == Branch)
				{
					this->recursive(static_cast<const OctreeBranch*>(node)->at(0), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(1), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(2), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(3), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(4), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(5), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(6), callback);
					this->recursive(static_cast<const OctreeBranch*>(node)->at(7), callback);
				}
				else if (node->type() == Leaf)
				{
					callback(node);
				}
				else
				{
					assert(false);
				}
			}

			std::size_t size() const
			{
				return _size;
			}

			void swap(Octree& other) noexcept
			{
				std::swap(_root, other._root);
				std::swap(_size, other._size);
			}

			OctreeNode& operator() (index_t x, index_t y, index_t z)
			{
				return this->at(x, y, z);
			}

			const OctreeNode& operator() (index_t x, index_t y, index_t z) const
			{
				return this->at(x, y, z);
			}

			bool load(std::istream& in)
			{
				Octree tmp;

				char root;
				in.read(&root, 1);

				if (root)
					this->loadRecursive(in, &tmp._root);

				in.read(reinterpret_cast<char*>(&tmp._empty_value), sizeof(tmp._empty_value));
				in.read(reinterpret_cast<char*>(&tmp._size), sizeof(tmp._size));

				if (in.good())
				{
					this->swap(tmp);
					return true;
				}

				return false;
			}

			bool save(std::istream& out)
			{
				static const char one = 1;
				static const char zero = 0;

				if (!_root)
				{
					out.write(&zero, 1);
				}
				else
				{
					out.write(&one, 1);

					this->saveRecursive(out, this->root());
				}

				out.write(reinterpret_cast<const char*>(&_empty_value), sizeof(_empty_value));
				out.write(reinterpret_cast<const char*>(&_size), sizeof(_size));
			}

		private:
			void loadRecursive(std::istream& in, OctreeNode** node)
			{
				assert(node);

				if (in.fail()) return;

				char type;
				in.read(&type, 1);

				switch (type)
				{
				case node_t::Branch:
				{
					OctreeBranch* branch = new OctreeBranch;
					*node = branch;

					char children;
					in.read(&children, 1);

					if (children & (1 << 0)) this->loadRecursive(in, &branch->at(0));
					if (children & (1 << 1)) this->loadRecursive(in, &branch->at(1));
					if (children & (1 << 2)) this->loadRecursive(in, &branch->at(2));
					if (children & (1 << 3)) this->loadRecursive(in, &branch->at(3));
					if (children & (1 << 4)) this->loadRecursive(in, &branch->at(4));
					if (children & (1 << 5)) this->loadRecursive(in, &branch->at(5));
					if (children & (1 << 6)) this->loadRecursive(in, &branch->at(6));
					if (children & (1 << 7)) this->loadRecursive(in, &branch->at(7));
				}
				break;
				case node_t::Leaf:
				{
					OctreeLeaf* leaf = new OctreeLeaf(T(0));
					*node = leaf;
					in.read(reinterpret_cast<char*>(&leaf->value()), sizeof(T));
				}
				break;
				default:

					break;
				}
			}

			void saveRecursive(std::ostream& in, OctreeNode** node)
			{
				assert(node);

				if (out.fail()) return;

				char type = node->type();
				out.write(&type, 1);

				switch (type)
				{
				case node_t::Branch:
				{
					const OctreeBranch* branch = static_cast<const OctreeBranch*>(node);

					char children = 0;
					children |= (branch->at(0) != 0) << 0;
					children |= (branch->at(1) != 0) << 1;
					children |= (branch->at(2) != 0) << 2;
					children |= (branch->at(3) != 0) << 3;
					children |= (branch->at(4) != 0) << 4;
					children |= (branch->at(5) != 0) << 5;
					children |= (branch->at(6) != 0) << 6;
					children |= (branch->at(7) != 0) << 7;

					out.write(&children, 1);

					if (branch->at(0)) this->saveRecursive(out, branch->at(0));
					if (branch->at(1)) this->saveRecursive(out, branch->at(1));
					if (branch->at(2)) this->saveRecursive(out, branch->at(2));
					if (branch->at(3)) this->saveRecursive(out, branch->at(3));
					if (branch->at(4)) this->saveRecursive(out, branch->at(4));
					if (branch->at(5)) this->saveRecursive(out, branch->at(5));
					if (branch->at(6)) this->saveRecursive(out, branch->at(6));
					if (branch->at(7)) this->saveRecursive(out, branch->at(7));
				}
				break;
				case node_t::Leaf:
				{
					out.write(reinterpret_cast<const char*>(&reinterpret_cast<const OctreeLeaf*>(node)->value()), sizeof(T));
				}
				break;
				default:
					break;
				}
			}

			std::size_t _count;
			std::unique_ptr<OctreeBranch> _root;

			size_t _size;
		};
	}
}

namespace std
{
	template<typename _Tx, typename _Ty>
	void swap(octoon::math::Octree<_Tx, _Ty>& a, octoon::math::Octree<_Tx, _Ty>& b)
	{
		a.swap(b);
	}
}

#endif