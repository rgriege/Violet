// ============================================================================

template <typename StreamDeserializer>
vlt::file_deserializer<StreamDeserializer>::file_deserializer(std::filebuf && _file) :
	file(std::move(_file)),
	stream(&file),
	inner_deserializer(stream)
{
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
vlt::file_deserializer<StreamDeserializer>::~file_deserializer()
{
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
bool vlt::file_deserializer<StreamDeserializer>::is_valid() const
{
	return inner_deserializer.is_valid();
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
std::unique_ptr<vlt::deserializer> vlt::file_deserializer<StreamDeserializer>::enter_segment(const char * label)
{
	return inner_deserializer.enter_segment(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
const char * vlt::file_deserializer<StreamDeserializer>::next_label() const
{
	return inner_deserializer.next_label();
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
bool vlt::file_deserializer<StreamDeserializer>::get_b8(const char * label)
{
	return inner_deserializer.get_b8(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
u32 vlt::file_deserializer<StreamDeserializer>::get_u32(const char * label)
{
	return inner_deserializer.get_u32(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
s32 vlt::file_deserializer<StreamDeserializer>::get_s32(const char * label)
{
	return inner_deserializer.get_s32(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
r32 vlt::file_deserializer<StreamDeserializer>::get_r32(const char * label)
{
	return inner_deserializer.get_r32(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
r64 vlt::file_deserializer<StreamDeserializer>::get_r64(const char * label)
{
	return inner_deserializer.get_r64(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
const char * vlt::file_deserializer<StreamDeserializer>::get_string(const char * label)
{
	return inner_deserializer.get_string(label);
}

// ============================================================================
